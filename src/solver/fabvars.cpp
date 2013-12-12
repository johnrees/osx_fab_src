#include <iostream>
#include <fstream>
#include <cstdlib>

#include <png.h>

#include "fabvars.hpp"
#include "region.hpp"

using namespace std;
FabVars::FabVars(output_mode o, int argc, char** argv)
    : ni(-1), nj(-1), nk(-1),
      min_volume(-1), min_area(-1),
      xmin(0), ymin(0), zmin(0),
      dx(0), dy(0), dz(0),
      pixels_per_mm(10), mm_per_unit(-1), quality(8),
      decimation_error(1),
      mode(SOLVE_BOOL), output(o), projection(false),
      infile_name(""), outfile_name(""),
      red(NULL), green(NULL), blue(NULL), intensity(NULL),
      volume(0)
{

    infile_name = argv[0];
    
    if (output != OUTPUT_STATS)
        outfile_name = argv[1];

    pixels_per_mm = 10;
        
    // The different programs have different input argument orders.
    if (output == OUTPUT_PNG) {
        if (argc >= 3)
            pixels_per_mm = atof(argv[2]);
        if (argc >= 4)
            nk = atoi(argv[3]);
    }
    else if (output == OUTPUT_STL) {
        if (argc >= 3)
            pixels_per_mm = atof(argv[2]);
        if (argc >= 4)
            quality = atoi(argv[3]);
    }
    else if (output == OUTPUT_SVG) {
        if (argc >= 3)
            pixels_per_mm = atof(argv[2]);
        if (argc >= 4)
            nk = atoi(argv[3]);
        if (argc >= 5)
            decimation_error = atof(argv[4]);
        if (argc >= 6)
            quality = atoi(argv[7]);
    }
    else if (output == OUTPUT_STATS) {
        if (argc >= 2)
            pixels_per_mm = atof(argv[1]);
    }
    // Load data from the input file
    load();
}

FabVars::~FabVars()
{
    if (mode == SOLVE_RGB && red && green && blue) {
        for (int y = 0; y < nj; ++y) {
            delete [] red[y];
            delete [] green[y];
            delete [] blue[y];
        }
        delete [] red;
        red = NULL;
        delete [] green;
        green = NULL;
        delete [] blue;
        blue = NULL;
    } else if ((mode == SOLVE_BOOL || mode == SOLVE_REAL) && intensity) {
        for (int y = 0; y < nj; ++y)
            delete [] intensity[y];
        delete [] intensity;
        intensity = NULL;
    }
}

void FabVars::load()
{
    if (infile_name == "") {
        cerr << "No input file provided!" << endl;
        exit(1);
    }
        
    fstream input;
    
     // Open the input file
    input.open(infile_name.c_str(), ios::in);
    if (!input.good()) {
        cerr << "Failed to open input file.\n";
        exit(1);
    }

    // Read and parse the file
    string line;
    while (getline(input, line))
        if (line.find("format:") != string::npos)
            if (line.find("Boolean") != string::npos)
                mode = SOLVE_BOOL;
            else if (line.find("RGB") != string::npos)
                mode = SOLVE_RGB;
            else if (line.find("Real") != string::npos)
                mode = SOLVE_REAL;
            else {
                cerr << "Input math string must be Boolean, RGB, or Real, not "
                     << line.substr(8,string::npos) << "." << endl;
                 exit(4);
            }
        else if (line.find("mm per unit:") != string::npos)
            sscanf(line.substr(12,string::npos).c_str(),"%lf",
                   &mm_per_unit);
        else if (line.find("dx dy dz:") != string::npos)
            sscanf(line.substr(9,string::npos).c_str(),"%lf %lf %lf",
                   &dx, &dy, &dz);
        else if (line.find("xmin ymin zmin:") != string::npos)
            sscanf(line.substr(15,string::npos).c_str(),"%lf %lf %lf",
                   &xmin, &ymin, &zmin);
        else if (line.find("expression: ") != string::npos)
            math_string = line.substr(12,string::npos);

    ni = dx * mm_per_unit * pixels_per_mm;
    nj = dy * mm_per_unit * pixels_per_mm;
    // Default Z slices: 10 for SVG output, equivalent resolution
    // for other output modes.
    if (nk == -1) {
        if (output == OUTPUT_SVG)
            nk = dz ? 10 : 1;
        else
            nk = dz * mm_per_unit * pixels_per_mm;
    }
    
    // Make sure that these are all non-zero.
    ni = ni ? ni : 1;
    nj = nj ? nj : 1;
    nk = nk ? nk : 1;
    pb.full = uint64_t(ni)*uint64_t(nj)*uint64_t(nk);

    // Pick a minimum volume below which we won't do
    // octree recursion.
    min_volume = 64;
    min_area   = 8;
    
    // Pick a stroke size for drawing SVGs
    stroke = min(ni, nj) / (pixels_per_mm * 1000.);
    
    // Convert decimation error in mm^2 into something that can
    // directly be compared with a simplified Heron's formula output.
    decimation_error = pow(decimation_error*4, 2);
    paths.decimation_error = decimation_error;
    
    // Everything below this point is allocating memory for the images,
    // so return early if we have a different output format.
    if (output != OUTPUT_PNG)
        return;
    
    // Allocate memory for the image.
    if (mode == SOLVE_RGB) {
        red = new uint8_t*[nj];
        green = new uint8_t*[nj];
        blue = new uint8_t*[nj];
        for (int y = 0; y < nj; ++y) {
            red[y] = new uint8_t[ni];
            green[y] = new uint8_t[ni];
            blue[y] = new uint8_t[ni];
            for (int x = 0; x < ni; ++x)
            {
                red[y][x] = 0;
                green[y][x] = 0;
                blue[y][x] = 0;
            }
        }
    } else {
        intensity = new uint16_t*[nj];
        for (int y = 0; y < nj; ++y) {
            intensity[y] = new uint16_t[ni];
            for (int x = 0; x < ni; ++x)
                intensity[y][x] = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

float FabVars::x(float i) const
{
    return xmin + dx*i / float(ni);
}

float FabVars::y(float j) const
{
    return ymin + dy*j / float(nj);
}

float FabVars::z(float k) const
{
    return zmin + dz*k / float(nk);
}

////////////////////////////////////////////////////////////////////////////////

FabInterval FabVars::x(float imin, float imax) const
{
    return FabInterval(xmin + dx * imin / float(ni),
                       xmin + dx * imax / float(ni));
}

FabInterval FabVars::y(float jmin, float jmax) const
{
    return FabInterval(ymin + dy * jmin / float(nj),
                       ymin + dy * jmax / float(nj));
}

FabInterval FabVars::z(float kmin, float kmax) const
{
    return FabInterval(zmin + dz * kmin / float(nk),
                       zmin + dz * kmax / float(nk));
}

float FabVars::scale(unsigned int k) const
{
    if (nk == 1) {
        if (mode == SOLVE_BOOL || mode == SOLVE_REAL)
            return 65535;
        else
            return 1;
    }
    
    return k/float(nk - 1) *
           ((mode == SOLVE_BOOL || mode == SOLVE_REAL) ? 65535 : 1);
}

////////////////////////////////////////////////////////////////////////////////

int FabVars::k(float z) const
{
    return (z - zmin) * float(nk) / dz;
}

////////////////////////////////////////////////////////////////////////////////

void FabVars::fill(Region r)
{
    float s = scale(r.kmax - 1);
    for (int i = r.imin; i < r.imax; ++i)
        for (int j = r.jmin; j < r.jmax; ++j)
            if (s > intensity[nj - j - 1][i])
                intensity[nj - j - 1][i] = s;
}

void FabVars::fill(Region r, unsigned char R, unsigned char G, unsigned char B)
{
    float s = scale(r.kmax);
    for (int j = r.jmin; j < r.jmax; ++j)
        for (int i = r.imin; i < r.imax; ++i) {
            if (R * s > red[nj - j - 1][i])
                red[nj - j - 1][i] = R * s;
            if (G * s > green[nj - j - 1][i])
                green[nj - j - 1][i] = G * s;
            if (B * s > blue[nj - j - 1][i])
                blue[nj - j - 1][i] = B * s;
        }
}

////////////////////////////////////////////////////////////////////////////////

void FabVars::add_triangles(std::list<Triangle> tris)
{
    geometry_lock.lock();
    triangles.splice(triangles.begin(), tris);
    geometry_lock.unlock();
}

void FabVars::add_paths(const PathSet& p)
{
    geometry_lock.lock();
    
    list<Path>::const_iterator it;
    for (it = p.begin(); it != p.end(); ++it)
        paths += *it;
        
    geometry_lock.unlock();
}

void FabVars::add_volume(const uint64_t v)
{
    geometry_lock.lock();
    volume += v;
    geometry_lock.unlock();
}
////////////////////////////////////////////////////////////////////////////////

void FabVars::write_png()
{
    if (mode == SOLVE_BOOL || mode == SOLVE_REAL)
        fab_write_png_K16(this, outfile_name.c_str());
    else if (mode == SOLVE_RGB)
        fab_write_png_RGB24(this, outfile_name.c_str());
}

void FabVars::write_stl()
{
    fstream stl_out;
    stl_out.open(outfile_name.c_str(), fstream::trunc | fstream::out);
    
    // The first 80 characters are undefined, so let's leave an informative message!
    stl_out << "This is a binary STL file created by math_stl. Learn more at kokompe.cba.mit.edu";
    stl_out << "    ";

    uint32_t stl_faces = 0;

    list<Triangle>::iterator it;
    for (it = triangles.begin(); it != triangles.end(); ++it)
    {
        // Write the normal as all zeros
        for (int i = 0; i < 12; ++i)
            stl_out << char(0);
    
        // Extract vertices from the list
        Vec3f v1 = it->v1;
        Vec3f v2 = it->v2;
        Vec3f v3 = it->v3;
        
        // Convert into world coordinates    
        v1.x = x(v1.x);
        v1.y = y(v1.y);
        v1.z = z(v1.z);
        // Write out each of the floats, byte by byte
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v1.x)[i];
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v1.y)[i];
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v1.z)[i];
            
        v2.x = x(v2.x);
        v2.y = y(v2.y);
        v2.z = z(v2.z);
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v2.x)[i];
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v2.y)[i];
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v2.z)[i];
            
        v3.x = x(v3.x);
        v3.y = y(v3.y);
        v3.z = z(v3.z);
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v3.x)[i];
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v3.y)[i];
        for (int i = 0; i < 4; ++i)
            stl_out << ((char*)&v3.z)[i];
    
        stl_out << char(0) << char(0);
        
        stl_faces++;
    }
    
    stl_out.seekp(80);
    for (int i = 0; i < 4; ++i)
        stl_out << ((char*)&stl_faces)[i];
        
    stl_out.close();
}

void FabVars::write_svg()
{
    float scale = pixels_per_mm * 0.352778; // 72 dpi

    fstream svg_out;
    svg_out.open(outfile_name.c_str(), fstream::trunc | fstream::out);
    svg_out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n"
            << "<!-- Created with math_svg (kokompe.cba.mit.edu) -->\n"
            << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"\n"
            << "    \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n"
            << "<svg \n"
            << "    xmlns=\"http://www.w3.org/2000/svg\"\n"
            << "    width=\""  << ni / pixels_per_mm << "mm\"\n"
            << "    height=\"" << nj / pixels_per_mm << "mm\"\n"
            << "    units=\"mm\"\n"
//            << "    xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n"
//            << "    inkscape:document-units=\"mm\""
            << "    viewBox=\"0 0 " 
            << ni / scale << ' ' << nj / scale << "\">\n"
            << "<g style=\"stroke:rgb(0,0,0); "
            << "stroke-width:" << stroke << "; "
            << "fill:none\">\n";

    list<Path>::iterator it;
    for (it = paths.begin(); it != paths.end(); ++it) {
        svg_out << "  <path d=\"M" << it->front().x / scale
                << ' ' << (nj - it->front().y - 1) / scale;
        Path::iterator p = it->begin();
        Path::iterator path_end = it->end();
        bool loop = (it->front() == it->back());
        if (loop)
            path_end--;
        while (++p != path_end)
            svg_out << " L" << p->x / scale
                    << ' ' << (nj - p->y - 1) / scale;
        if (loop)
            svg_out << " Z";
        svg_out << "\"/>\n";
    }

    svg_out << "</g>\n</svg>";
    svg_out.close();
}
// Based on code from math_png
// with attribution
// Neil Gershenfeld
// CBA MIT 3/6/11
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

void fab_write_png_K16(FabVars *v, const char* output_file_name) {
    //
    // write 16-bit grayscale PNG from FabVars
    //
	FILE *output_file;
    int x,y;
    png_uint_32 res_x,res_y;
    png_byte color_type;
    png_byte bit_depth;
    png_byte *ptr;
    //
    // open PNG file
    //
	output_file = fopen(output_file_name, "wb");
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                         NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, output_file);
    //
    // set vars
    //
    bit_depth = 16;
    color_type = PNG_COLOR_TYPE_GRAY; 
	png_set_IHDR(png_ptr, info_ptr, v->ni, v->nj,
	             bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    res_x = 1000 * v->ni / (v->dx * v->mm_per_unit);
    res_y = 1000 * v->nj / (v->dy * v->mm_per_unit);
    png_set_pHYs(png_ptr, info_ptr, res_x, res_y, PNG_RESOLUTION_METER);
    
    png_text text[2];
        
    char zmin[10];
    snprintf(zmin, 10, "%g", v->zmin*v->mm_per_unit);
    text[0].compression = PNG_TEXT_COMPRESSION_NONE;
    text[0].key = (char*)"zmin";
    text[0].text = zmin;

    char zmax[10];
    snprintf(zmax, 10, "%g", (v->zmin+v->dz)*v->mm_per_unit);
    text[1].compression = PNG_TEXT_COMPRESSION_NONE;
    text[1].key = (char*)"zmax";
    text[1].text = zmax;
    png_set_text(png_ptr, info_ptr, text, 2);
    
    png_write_info(png_ptr, info_ptr);
    //
    // allocate pixels
    //
	png_bytep* row_pointers = new png_bytep[v->nj];
	for (y = 0; y < v->nj; ++y)
	    row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];
	    
    //
    // set pixels
    //
    for (y = 0; y < v->nj; ++y)
        for (x = 0; x < v->ni; ++x) {
    		ptr = &(row_pointers[y][x*2]);
    	    ptr[0] = (v->intensity[y][x] >> 8) & 255;
    	    ptr[1] = v->intensity[y][x] & 255;
    	}
    //
    // write, close, and return
    //
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);
    fclose(output_file);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    cout << "write " << output_file_name
         << "\n    x pixels: " << v->ni << ", y pixels: " << v->nj
         << "\n    x pixels/m: " << res_x << ", y pixels/m: " << res_y
         << "\n    dx: " << v->dx << " mm, dy: " << v->dy <<" mm" << endl;
    
	for (y = 0; y < v->nj; ++y)
	    delete [] row_pointers[y];
	delete [] row_pointers;
}

void fab_write_png_RGB24(FabVars *v, const char *output_file_name) {
    //
    // write 24-bit RGB PNG from FabVars
    //
	FILE *output_file;
    int x,y;
    png_uint_32 res_x,res_y;

    png_byte color_type;
    png_byte bit_depth;
    png_byte *ptr;
    //
    // open PNG file
    //
	output_file = fopen(output_file_name, "wb");
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, output_file);
    //
    // set vars
    //
    bit_depth = 8;
    color_type = PNG_COLOR_TYPE_RGB; 
	png_set_IHDR(png_ptr, info_ptr, v->ni, v->nj,
	    bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    res_x = 1000 * v->ni / (v->dx * v->mm_per_unit);
    res_y = 1000 * v->nj / (v->dy * v->mm_per_unit);
    png_set_pHYs(png_ptr, info_ptr, res_x, res_y, PNG_RESOLUTION_METER);
    png_write_info(png_ptr, info_ptr);
    //
    // allocate pixels
    //
    png_bytep* row_pointers = new png_bytep[v->nj];
	for (y = 0; y < v->nj; ++y)
		row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];
    //
    // set pixels
    //
    for (y = 0; y < v->nj; ++y)
        for (x = 0; x < v->ni; ++x) {
    		ptr = &(row_pointers[y][x*3]);
    	    ptr[0] = v->red[y][x];
    	    ptr[1] = v->green[y][x];
    	    ptr[2] = v->blue[y][x];
    	    }
    //
    // write, close, and return
    //
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);
    fclose(output_file);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    cout << "write " << output_file_name
         << "\n    x pixels: " << v->ni << ", y pixels: " << v->nj
         << "\n    x pixels/m: " << res_x << ", y pixels/m: " << res_y
         << "\n    dx: " << v->dx << " mm, dy: " << v->dy <<" mm" << endl;
    
	for (y = 0; y < v->nj; ++y)
	    delete [] row_pointers[y];
	delete [] row_pointers;
}

