reset
set style fill solid 0.5 border -1
set style boxplot outliers pointtype 7
set style data boxplot
set boxwidth  0.5
set pointsize 0.5

set xlabel 'method'
set ylabel 'time(msec)'
set xtics ("malloc" -3, "pool 0x1000" -1, \
           "pool 0x500" 1, "pool 0x5000" 3) scale 0.0
set xtics nomirror
set ytics nomirror
set terminal png font " Times_New_Roman,12 "
set output 'out.png'


plot [:]\
"out.txt" using (-3):1 title " ", \
"mem.txt" using (-1):1 title " ", \
"mem500.txt" using (1):1 title " ", \
"mem5000.txt" using (3):1 title " ", \

