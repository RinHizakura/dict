reset
set style fill solid 0.5 border -1
set style boxplot outliers pointtype 7
set style data boxplot
set boxwidth  0.5
set pointsize 0.5

set xlabel 'method'
set ylabel 'time(ms)'
set xtics ("using malloc" 1, "using memory pool" 2) scale 0.0
set xtics nomirror
set ytics nomirror
set terminal png font " Times_New_Roman,12 "
set output 'out.png'


plot \
"out.txt" using (1):2 title " ", \
"mem.txt" using (2):2 title " ", \

