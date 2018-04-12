echo "Generating Git commit log..."
OUTPUT="gitlog.tex"
git log --reverse --pretty=format:"%an & %ad & %s
" --date=short > gitlog1.md
echo "\begin{center}
\begin{longtabu} to \textwidth {|
    X[4,l]|
    X[3,c]|
    X[8,l]|}
    \hline
    \textbf{Author} & \textbf{Date} & \textbf{Message} \\ \hline" > $OUTPUT

awk '{printf "|%s\n",$0}' < gitlog1.md >> gitlog2.md
pandoc gitlog2.md -o gitlog1.tex
tr '\n' ' ' < gitlog1.tex > gitlog2.tex
sed -i -e 's/\\textbar{}/\'$'\n/g' gitlog2.tex
sed '/^ $/d' gitlog2.tex > gitlog3.tex
sed '/^$/d' gitlog3.tex > gitlog2.tex
awk '{printf "%s\\\\ \\hline\n",$0}' < gitlog2.tex >> gitlog2
sed -i -e 's/\\&/\&/g' gitlog2
cat gitlog2 >> $OUTPUT
echo "\end{longtabu}
\end{center}" >> $OUTPUT
rm gitlog1* gitlog2* gitlog3*
echo "Finished generating commit log!"
