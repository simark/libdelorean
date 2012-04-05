# documentation generation
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>
XMLTO = xmlto
DOC = librbrntrvll.dbk
CFG = librbrntrvll_cfg.xsl
IMGDIR = img/export
SCRIPTSDIR = scripts
CSSDIR = css
OUTDIR = out
OUTIMGDIR = $(OUTDIR)/img
OUTSCRIPTSDIR = $(OUTDIR)/scripts
OUTCSSDIR = $(OUTDIR)/css
OUTINDEX = $(OUTDIR)/index.html
OUTCSS = $(OUTDIR)/$(CSS)
RM = rm -f
RMDIR = rm -rf
CP = cp

.PHONY: clean allcheck all imgs scripts css

allcheck:
	for d in $(OUTDIR) $(OUTIMGDIR) $(OUTSCRIPTSDIR) $(OUTCSSDIR); do \
		if [ ! -d $$d ]; then mkdir $$d; fi \
	done
	make all

all: $(OUTINDEX) imgs scripts css

$(OUTINDEX): $(DOC) $(CFG)
	$(XMLTO) -o $(OUTDIR) -m $(CFG) html $(DOC)

$(OUTCSS): $(CSS)
	$(CP) $< $@

imgs:
	$(CP) $(IMGDIR)/*.png $(OUTIMGDIR)

scripts:
	$(CP) $(SCRIPTSDIR)/*.js $(OUTSCRIPTSDIR)

css:
	$(CP) $(CSSDIR)/*.css $(OUTCSSDIR)

clean:
	$(RMDIR) $(OUTDIR)

