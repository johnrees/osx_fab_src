# The lines below extract out program names from the cmake cache
# This is used to automatically generate a documentation page.
C_ = $(shell sed -n -e 's/;/ /g'                                \
                    -e 's/SOLVER_EXECUTABLES:STRING=//gp'       \
                    -e 's/PROGRAMS:STRING=//gp'                 \
                    build/CMakeCache.txt)
C  = $(addprefix bin/, $(C_))

Python_ = $(shell sed -n -e 's/;/ /g'                           \
                         -e 's/PYs:STRING=//gp'                 \
                         build/CMakeCache.txt)
Python  = $(addprefix bin/, $(Python_))


scripts_ = $(shell sed -n -e 's/;/ /g'                          \
                          -e 's/SCRIPTS:STRING=//gp'            \
                          build/CMakeCache.txt)
scripts  = $(addprefix bin/, $(scripts_))

GUIs_  = $(shell sed -n -e 's/;/ /g'                            \
                        -e 's/GUIs:STRING=//gp'                 \
                        build/CMakeCache.txt)
GUIs   = $(addprefix bin/, $(GUIs_))

PWD := $(shell pwd)

help:
	@echo "Makefile options:"
	@echo "    make fab           Compile all files and copy scripts from src to bin"
	@echo "    make doc           Saves command names and docstrings into commands.html"
	@echo "    make zip           Bundles relevant files in fab.zip"
	@echo "    make dist          Copies files to Web directory"
	@echo "    make install       Copies files to /usr/local/bin"
	@echo "    make clean         Removes compiled executables and scripts from bin"
	@echo "    make wxpython2.9   Downloads, compiles, and installs wxpython 2.9.4.1"
	@echo "                       (Linux only)"

fab:
	@echo "Building with CMake"
	@mkdir -p build
	
	@cd build;                                         \
	 cmake ../src;                                     \
	 make -j4;                                         \
	 make install | sed "s@$(PWD)/src/../@@g"

doc: commands.html
commands.html: fab
	@# Dump all of the command names
	@echo "	Storing command names"
	@echo "<html>\n<body>\n<pre>\ncommands:" > commands.html
	@for name in $(C) $(scripts) $(GUIs); do               \
	    echo "   "$$name >> commands.html;                 \
	done
	  
	@echo "" >> commands.html
	
	@# Dump command docstrings
	@echo "	Storing command docstrings"
	@for name in $(C) $(scripts) ; do                        \
	   ./$$name >> commands.html;                           \
	   echo "" >> commands.html;                            \
	done


zip: commands.html
	rm -f fab_src.zip
	rm -rf src/apps/dist
	rm -rf src/apps/build
	
#	@echo "Copying revision number to kokopelli About panel"
#	@if which hg &>/dev/null && hg summary &> /dev/null; \
#	 then \
#	    sed "s/CHANGESET = .*/CHANGESET = '`hg id --num`:`hg id --id`'/g" \
#	    src/guis/koko/__init__.py > tmp; \
#	    mv tmp src/guis/koko/__init__.py; \
#	 fi
	
	zip -r fab_src.zip commands.html Makefile src
	
#	@sed "s/CHANGESET = .*/CHANGESET = None/g" \
#	    src/guis/koko/__init__.py > tmp; \
#	    mv tmp src/guis/koko/__init__.py; \

dist: zip
	cp fab_src.zip ../../Web/fab_src.zip
	cp commands.html ../../Web/
	sed -e "s/Snapshot from [^\)]*/Snapshot from `date '+%B %d, %Y, %I:%M%p'`/g" \
	    ../../Web/downloads.html > ../../Web/_downloads.html
	mv ../../Web/_downloads.html ../../Web/downloads.html

install: fab
	@echo "Installing executables and scripts to /usr/local/bin"
	@if [ -e "/usr/local/bin/fab_send" ]; \
	then \
	    mv /usr/local/bin/fab_send /usr/local/bin/fab_send.old; \
	fi
	@cp -r bin/* /usr/local/bin/
	@if [ -e "/usr/local/bin/fab_send.old" ]; \
	then \
	    mv /usr/local/bin/fab_send /usr/local/bin/fab_send.new; \
	    mv /usr/local/bin/fab_send.old /usr/local/bin/fab_send; \
	    echo "Note:"; \
	    echo "   Pre-existing fab_send has not been overwritten, and"; \
	    echo "   the new version of fab_send has been named fab_send.new"; \
	fi

clean:
	@echo "Deleting build directory"
	@rm -rf build
	
