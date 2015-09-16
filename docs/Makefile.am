SUFFIXES = .pod .1

man_MANS = faextract.1 fasizesort.1 faalphabetfilter.1

EXTRA_DIST = $(man_MANS) faextract.pod fasizesort.pod faalphabetfilter.pod

PODARGS = --center='User Manuals'

.pod.1:
	$(POD2MAN) $(PODARGS) $< > $@ && touch $@

## Maintainer parano checks
parano:
	(for p in *.pod; do podchecker --warnings --warnings $$p; done)
