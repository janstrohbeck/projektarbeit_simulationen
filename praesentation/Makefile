.PHONY: pdf
.PHONY: clean

TARGET = Foliensatz
PROJECT = $(shell basename $(shell pwd))
BUILD_DIR = ./build

LATEXRUN = latexrun --bibtex-cmd biber --latex-args="--shell-escape" -O $(BUILD_DIR)
VIEWER_PDF = evince

RM = rm -rf

all: uml pdf

uml:
	make -C ../uml epss

pdf:
	$(LATEXRUN) $(TARGET).tex

clean:
	$(RM) $(BUILD_DIR)

view: pdf
	$(VIEWER_PDF) $(TARGET).pdf &
