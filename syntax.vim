" symlink this file into your ~/.vim/syntax/src.vim
" ln -fs ~/I/syntax.vim ~/.vim/syntax/src.vim

syntax match Comment	"\v#.*$"
syntax match Number		"\v0x[0-9a-fA-f]+"
syntax match Operator	"\v[\+\-\*\/\^\=\@]"
syntax match Special	"\v[\(\)\[\]\:]"
syntax match Keyword	"\v(sym|num|op|vector|env|global)"
