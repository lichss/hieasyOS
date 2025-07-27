set number
set tabstop=3
set shiftwidth=2


" INSERT mode
let &t_SI = "\<Esc>[6 q" . "\<Esc>]12;white\x7"
" REPLACE mode
let &t_SR = "\<Esc>[3 q" . "\<Esc>]12;white\x7"
" NORMAL mode
let &t_EI = "\<Esc>[2 q" . "\<Esc>]12;white\x7"

inoremap <C-h> <Left>
inoremap <C-j> <Down>
inoremap <C-k> <Up>
inoremap <C-l> <Right>

nnoremap ! ^
nnoremap @ $

" 在 Insert 模式下用 Ctrl+P 粘贴系统剪贴板内容
inoremap <C-p> <C-r>+

" === 按键重映射 ===
" 将 q 完全替换为 b 的功能
nnoremap q b
onoremap q b
xnoremap q b

" 交换 o 和 O 的功能
nnoremap o O
nnoremap O o

" 将宏录制功能迁移到 \q
" nnoremap <Leader>q q
" xnoremap <Leader>q q

" " 可选：恢复 b 的原生功能到 \b
" nnoremap <Leader>b b
" onoremap <Leader>b b
" xnoremap <Leader>b b