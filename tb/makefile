.PHONY: tags

tags:
	rm -f tmptags
	ctags -R  -o tmptags . ../third_party ../mr
	ctags -R  --links=no --languages=c,c++ --c-kinds=+p -a -o tmptags /System/Library/Frameworks
	rm -f tags
	mv tmptags tags
