SUFFIXES = .pod .1

man_MANS = fastaextract.1 fasizesort.1 faalphabetsort.1

EXTRA_DIST = $(man_MANS) fastaextract.pod fasizesort.pod faalphabetsort.pod

PODARGS = --center='User Manuals'

.pod.1:
	$(POD2MAN) $(PODARGS) $< > $@ && touch $@

## Maintainer parano checks
parano:
	(for p in *.pod; do podchecker --warnings --warnings $$p; done)
