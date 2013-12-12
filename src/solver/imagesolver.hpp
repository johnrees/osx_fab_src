#ifndef IMAGESOLVER_H
#define IMAGESOLVER_H

#include <list>

#include "solver.hpp"

class ImageSolver : public Solver
{
public:
    /*  ImageSolver(FabVars& v)
     *  ImageSolver(MathTree* tree, FabVars& v)
     *
     *  Constructs an ImageSolver instance.
     */
    ImageSolver(FabVars& v);
    ImageSolver(MathTree* tree, FabVars& v);
    virtual ~ImageSolver() { /* Nothing to do here */ }

    /* virtual void evaluate_region(Region R)
     *  
     *  Evaluate a given region recursively, saving results wherever is
     *  appropriate.
     */
    virtual void evaluate_region(Region R);

    /* void evaluate_points(Region R)
     *  
     *  Evaluate a region in space
     */
    void evaluate_points(Region R);

    /*  void cull_regions(list<Region>& subregions)
     *
     *  Deletes a regions that can no longer affect the output
     *  image (i.e. anything that is darker than the existing value
     *  in that location).
     */
    void cull_regions(std::list<Region>& subregions);
    
    /* virtual void save()
     *  
     *  Nothing needs to happen here, since evaluate_region and
     *  evalue_points dump data into the image.
     */
    void save() { /* Nothing to do here */ }
};
#endif