# documentation generation
#
# author:	Philippe Proulx <philippe.proulx@polymtl.ca>
XMLTO = xmlto
DOC = librbrntrvll.dbk
CFG = librbrntrvll_cfg.xsl
OUTDIR = out
OUTINDEX = $(OUTDIR)/index.html
RM = rm -f

.PHONY: clean

$(OUTINDEX): $(DOC)
	$(XMLTO) -o $(OUTDIR) -m $(CFG) html $<

clean:
	$(RM) $(OUTDIR)/*.html

