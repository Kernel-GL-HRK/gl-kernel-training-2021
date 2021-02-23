#!/bin/bash 



var=""

 echo "Please choose: rock (r) - paper (p) - scissors (s) - exit (e)"

select ch in "r" "p" "s" "e"

do

	if [[ $ch == "e" ]]; then

	    echo "Bye"
	    break

	fi

	echo $ch

	rand=$((1 + $RANDOM % 3))

	case $rand in
	1) echo "You choose $ch, I choose rock"
			var="r"
			;;
	2) echo "You choose $ch, I choose scissors"
			var="s"
			;;
	3) echo "You choose $ch, I choose paper"
			var="p"
			;;
	*) "Error!";  exit 1 ;;
	esac
	
	if [[ $var == $ch ]]; then
	    echo "Its a Tie"
	elif [[ $var ==  "r" && $ch == "s" ]]; then
	    echo "you loose"
	elif [[ $var == "s" && $ch == "r" ]]; then
	    echo "You win "
	elif [[ $var == "s" && $ch == "p" ]]; then
	    echo "You loose"
	elif [[ $var == "p" && $ch == "s" ]]; then
	    echo "You win"
	elif [[ $var == "p" && $ch == "r" ]]; then
	    echo "You loose"
	elif [[ $var = "r" && $ch == "p" ]]; then
	    echo "You win"
	elif [[ $var = "r" && $ch == "p" ]]; then
	    echo "You win"
	else
	    echo "There was an error please try again"

    fi

done










