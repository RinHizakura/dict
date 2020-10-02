reset
set style fill solid
set xlabel 'experiment'
set ylabel 'time(sec)'
set terminal png font " Times_New_Roman,12 "
set output 'out.png'


plot [:][:0.25]\
"nobloom_out_cpy.txt" using 1:2 with points pt 6 title "no bloom CPY", \
"nobloom_out_ref.txt" using 1:2 with points pt 9 title "no bloom REF", \
"bloom_out_cpy.txt" using 1:2 with points  pt 7 title "bloom CPY"  , \
"bloom_out_ref.txt" using 1:2 with points title "bloom REF", \

