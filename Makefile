# Build mode
# 0: development (max safety, no optimisation)
# 1: release (min safety, optimisation)
# 2: fast and furious (no safety, optimisation)
BUILD_MODE?=1

all: pbmake_wget main validation generateRgbHsv generateDiabeteRisk generateAmphibian generateHCV generateAnnealing
	
# Automatic installation of the repository PBMake in the parent folder
pbmake_wget:
	if [ ! -d ../PBMake ]; then wget https://github.com/BayashiPascal/PBMake/archive/master.zip; unzip master.zip; rm -f master.zip; sed -i '' 's@ROOT_DIR=.*@ROOT_DIR='"`pwd | gawk -F/ 'NF{NF-=1};1' | sed -e 's@ @/@g'`"'@' PBMake-master/Makefile.inc; mv PBMake-master ../PBMake; fi

# Makefile definitions
MAKEFILE_INC=../PBMake/Makefile.inc
include $(MAKEFILE_INC)

# Rules to make the executable
repo=neuramorph
$($(repo)_EXENAME): \
		$($(repo)_EXENAME).o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) $($(repo)_EXENAME).o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) -o $($(repo)_EXENAME) 
	
$($(repo)_EXENAME).o: \
		$($(repo)_DIR)/$($(repo)_EXENAME).c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c $($(repo)_DIR)/$($(repo)_EXENAME).c

validation: \
		validation.o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) validation.o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) -o validation 

validation.o: \
		$($(repo)_DIR)/validation.c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c $($(repo)_DIR)/validation.c

generateRgbHsv: \
		generateRgbHsv.o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) generateRgbHsv.o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) -o generateRgbHsv 

generateRgbHsv.o: \
		$($(repo)_DIR)/generateRgbHsv.c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c $($(repo)_DIR)/generateRgbHsv.c

generateDiabeteRisk: \
		generateDiabeteRisk.o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) generateDiabeteRisk.o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) -o generateDiabeteRisk 

generateDiabeteRisk.o: \
		$($(repo)_DIR)/generateDiabeteRisk.c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c $($(repo)_DIR)/generateDiabeteRisk.c

generateHCV: \
		generateHCV.o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) generateHCV.o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) -o generateHCV 

generateHCV.o: \
		$($(repo)_DIR)/generateHCV.c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c $($(repo)_DIR)/generateHCV.c

generateAnnealing: \
		generateAnnealing.o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) generateAnnealing.o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) -o generateAnnealing 

generateAnnealing.o: \
		$($(repo)_DIR)/generateAnnealing.c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c $($(repo)_DIR)/generateAnnealing.c

generateAmphibian: \
		generateAmphibian.o \
		$($(repo)_EXE_DEP) \
		$($(repo)_DEP)
	$(COMPILER) `echo "$($(repo)_EXE_DEP) generateAmphibian.o" | tr ' ' '\n' | sort -u` $(LINK_ARG) $($(repo)_LINK_ARG) -o generateAmphibian 

generateAmphibian.o: \
		$($(repo)_DIR)/generateAmphibian.c \
		$($(repo)_INC_H_EXE) \
		$($(repo)_EXE_DEP)
	$(COMPILER) $(BUILD_ARG) $($(repo)_BUILD_ARG) `echo "$($(repo)_INC_DIR)" | tr ' ' '\n' | sort -u` -c $($(repo)_DIR)/generateAmphibian.c

latexValid:
	cd Validation; pdflatex -synctex=1 -interaction=nonstopmode -shell-escape  doc.tex; cd ..
