// Simplify calling gnuplot for this case
// 
//
//
// Some useful gnuplot commands
// gnuplot -persist  // keeps plot open
// gnuplot> set title <"title">
// gnuplot> set style data linepoints
// gnuplot> plot <'filename'>
//
// Consider this solution for "real time"
// 1. run file to get all results in one file
// 2. after sim is done, create file with partial data from existing file
// 3. update plot
// 4. ??? working ???:
//
// http://hxcaine.com/blog/2013/02/28/running-gnuplot-as-a-live-graph-with-automatic-updates/
