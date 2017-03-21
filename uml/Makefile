OUTPUT_FOLDER=build

FILES=$(wildcard *.plantuml)
SVGS=$(addprefix $(OUTPUT_FOLDER)/,$(FILES:.plantuml=.svg))
EPSS=$(addprefix $(OUTPUT_FOLDER)/,$(FILES:.plantuml=.eps))
PNGS=$(addprefix $(OUTPUT_FOLDER)/,$(FILES:.plantuml=.png))
PDFS=$(addprefix $(OUTPUT_FOLDER)/,$(FILES:.plantuml=.pdf))

PLANTUML=plantuml
FLAGS_SVG=-tsvg
FLAGS_EPS=-teps
FLAGS_PNG=-tpng
FLAGS_COMMON=-o $(OUTPUT_FOLDER)
EPSTOPDF=epstopdf

all: svgs epss pngs pdfs

.PHONY: svgs
svgs: $(SVGS)

.PHONY: epss
epss: $(EPSS)

.PHONY: pngs
pngs: $(PNGS)

.PHONY: pdfs
pdfs: $(PDFS)

$(OUTPUT_FOLDER)/%.svg: %.plantuml
	$(PLANTUML) $(FLAGS_SVG) $(FLAGS_COMMON) $<

$(OUTPUT_FOLDER)/%.eps: %.plantuml
	$(PLANTUML) $(FLAGS_EPS) $(FLAGS_COMMON) $<

$(OUTPUT_FOLDER)/%.png: %.plantuml
	$(PLANTUML) $(FLAGS_PNG) $(FLAGS_COMMON) $<

$(OUTPUT_FOLDER)/%.pdf: $(OUTPUT_FOLDER)/%.eps
	$(EPSTOPDF) --outfile=$@ $<
