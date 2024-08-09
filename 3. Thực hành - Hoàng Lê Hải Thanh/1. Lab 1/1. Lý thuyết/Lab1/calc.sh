#! /bin/bash

declare -a hist

read -p ">> " num1 operator num2

if [ -f ans.txt ]
then
	read store < ans.txt
else
	store=0
fi

while [ "$num1" != "EXIT" ]
do

if [ "$num1" = "HIST" ]
then
	size=${#hist[@]}
	for ((i = 0; i <= $size; i++))
	do
		echo ${hist[$i]}
	done
elif [[ ! "$num1" =~ ^-?[0-9]*(\.[0-9]+)?$|ANS ]] || [[ ! "$num2" =~ ^-?[0-9]*(\.[0-9]+)?$|ANS ]] || [[ ! "$operator" =~ [+x/%-] ]]
then
	echo -e "SYNTAX ERROR"
elif ([ "$operator" = "/" ] || [ "$operator" = "%" ]) && [ $num2 = 0 ]
then
	echo -e "MATH ERROR"
else
	ans_flag=-1
	if [ "$num1" = "ANS" ] && [ "$num2" = "ANS" ]
	then
		num1=$store
		num2=$store
		ans_flag=0
	fi

	if [ "$num1" = "ANS" ]
	then
		num1=$store
		ans_flag=1
	fi
	
	if [ "$num2" = "ANS" ]
	then
		num2=$store
		ans_flag=2
	fi
		
	case $operator in
	"+")res=`echo $num1 + $num2 | bc`
	;;
	"-")res=`echo $num1 - $num2 | bc`
	;;
	"x")res=`echo $num1 \* $num2 | bc`
	;;
	"/")res=`echo "scale=2; $num1 / $num2" | bc`
	;;
	"%")res=`echo "scale=0; $num1 / $num2" | bc`
	;;
	esac

	if [[ ! "$res" =~ ^-?[0-9]+$ ]]
	then
		res=`printf "%.2f" $res`
	fi

	case $ans_flag in
	0)num1=ANS
	num2=ANS
	;;
	1)num1=ANS
	;;
	2)num2=ANS
	;;
	esac
	
	store=$res
	echo $store > ans.txt

	hist+=("$num1 $operator $num2 = $res")

	if [ ${#hist[@]} -gt 5 ]; then
		hist=("${hist[@]:1}")	
	fi
	echo -e "$res"
fi

read -n 1
clear
read -p ">> " num1 operator num2

done