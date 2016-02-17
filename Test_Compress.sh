#!/bin/bash

errorOccurred=0

cd input_files/

rm core* 2>/dev/null
rm out* 2>/dev/null


for filename in *; do

  echo -e "===================================================================="
  echo -e "Compressing $filename..."
  printf "===================================================================="

  if [ "$filename" == "empty" ]; then

    echo -e ""
    echo `../compress $filename out.txt`
    echo -e ""

    continue
  fi

  `valgrind ../compress $filename out.txt`
  size1=$(stat -c %s out.txt)
  `valgrind ../refcompress $filename out3.txt &>/dev/null`
  size2=$(stat -c %s out3.txt)

  `../uncompress out.txt out2.txt`
  success=$(diff -q $filename out2.txt)

  # $? is stores the exit value of the last command that was executed
  if [ $? -eq 0 ]; then
    echo -e "\nSUCCESS: The uncompressed file matches the original."
    difference=`expr $size2 - $size1`

    if [ $difference -le 1 ]; then

      errorOccurred=1

    fi

    echo -e "The compressed file is $difference bytes smaller than the solution's.\n"
  else
    echo -e "\nERROR: The uncompressed file differs from the original."
    errorOccurred=1
  fi

  rm core* 2>/dev/null
  rm out* 2>/dev/null

done

cd ../

if [ $errorOccurred -eq 1 ]; then

  echo -e "TEST FAILED. AT LEAST ONE FILE WAS NOT COMPRESSED CORRECTLY."

else

  echo -e "ALL TESTS PASSED."

fi
