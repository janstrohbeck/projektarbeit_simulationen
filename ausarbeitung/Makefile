.PHONY: pdf
.PHONY: clean

TARGET = Ausarbeitung
PROJECT = $(shell basename $(shell pwd))
BUILD_DIR = ./build

LATEXRUN = latexrun --bibtex-cmd biber --latex-args="--shell-escape" -O $(BUILD_DIR)
VIEWER_PDF = evince

RM = rm -rf

all: pdf

uml:
	make -C ../uml pdfs

pdf: uml
	$(LATEXRUN) $(TARGET).tex

clean:
	$(RM) $(BUILD_DIR)

view: pdf
	$(VIEWER_PDF) $(TARGET).pdf &
