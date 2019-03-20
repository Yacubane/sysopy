echo "---- Test less than 2018-03-14 21:34 ----"
echo "==== opendir ===="
./main_dir test_files \< 2018-03-14 21:34
echo "==== nftw ===="
./main_nftw test_files \< 2018-03-14 21:34
echo ""

echo "---- Test equal 2014-01-01 12:00 ----"
echo "==== opendir ===="
./main_dir test_files = 2014-01-01 12:00
echo "==== nftw ===="
./main_nftw test_files = 2014-01-01 12:00
echo ""

echo "---- Test more than 2018-03-14 21:34 ----"
echo "==== opendir ===="
./main_dir test_files \> 2018-03-14 21:34
echo "==== nftw ===="
./main_nftw test_files \> 2018-03-14 21:34
echo ""
