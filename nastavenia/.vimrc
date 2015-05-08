set guioptions-=T " Removes top toolbar
set guioptions-=r " Removes right hand scroll bar
set go-=L " Removes left hand scroll bar
set linespace=2
 

set smartindent
set nowrap                      " don't wrap lines
set tabstop=4                   " a tab is four spaces
set smarttab
set tags=tags
set softtabstop=4               " when hitting <BS>, pretend like a tab is removed, even if spaces
set expandtab                   " expand tabs by default (overloadable per file type later)
set shiftwidth=4                " number of spaces to use for autoindenting
set shiftround                  " use multiple of shiftwidth when indenting with '<' and '>'
set backspace=indent,eol,start  " allow backspacing over everything in insert mode
set autoindent                  " always set autoindenting on
set copyindent                  " copy the previous indentation on autoindenting
set number                      " always show line numbers
set ignorecase                  " ignore case when searching
set smartcase                   " ignore case if search pattern is all lowercase,
set timeout timeoutlen=200 ttimeoutlen=100
set noerrorbells         " don't beep
set autowrite  "Save on buffer switch
set mouse=a


" Searching
set incsearch
set hlsearch

" <Ctrl-l> redraws the screen and removes any search highlighting.
nnoremap <silent> <C-l> :nohl<CR><C-l>

" Incremental search plugin
map /  <Plug>(incsearch-forward)
map ?  <Plug>(incsearch-backward)

" Fast saves
nmap <leader>w :w!<cr>

"Easy escaping to normal model
imap jj <esc>
"Easier window navigation
map <C-n> :NERDTreeToggle<CR> 

" Pathogen for easy vim plugin installation
execute pathogen#infect()
syntax on

" Autoopen NERDTree when no files were specified
autocmd StdinReadPre * let s:std_in=1
autocmd VimEnter * if argc() == 0 && !exists("s:std_in") | NERDTree | endif
" Close whole vim if NERDTree is the only left opened window
autocmd bufenter * if (winnr("$") == 1 && exists("b:NERDTreeType") && b:NERDTreeType == "primary") | q | endif


set statusline=%<\ %F%=\ %{&fileformat}\ \|\ %{&fileencoding}\ \|\ %{&filetype}\ \|\ %p%%\ \|\ LN\ %l:%c\
set showmode
"set rtp+=/usr/local/lib/python2.7/dist-packages/powerline/bindings/vim/
" Powerline (Fancy thingy at bottom stuff)
"let g:Powerline_symbols = 'fancy'
"set laststatus=2   " Always show the statusline
"set encoding=utf-8 " Necessary to show Unicode glyphs
"set noshowmode " Hide the default mode text (e.g. -- INSERT -- below the statusline)


