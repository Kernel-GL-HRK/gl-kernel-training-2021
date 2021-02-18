#!/bin/bash

# Rock-paper-scissors console game

# An array from which random values will be selected 
options=(rock paper scissors)

echo "Rock-paper-scissors"
echo "Please choose: rock (r) - paper (p) - scissors (s)"
echo "Enter 'q' or 'quit' for exit"

while :
do
	echo ""
	read enemy_choise

	# Generate random value from 0 to 3 and get option
	rand=$((RANDOM%3))
	choise=${options[rand]}

	case $enemy_choise in
		r)
			echo You choose rock, I choose $choise
			if [[ $choise = rock ]]
			then
				echo Draw: two rocks!
			elif [[ $choise = scissors ]]
			then
				echo You win: rock beats $choise!
			elif [[ $choise = paper ]]
			then
				echo I win: $choise beats rock!
			fi
		;;
		p)
			echo You choose paper, I choose $choise
			if [[ $choise = paper ]]
              	 	then
				echo Draw: two paper!
			elif [[ $choise = rock ]]
			then
				echo You win: paper beats $choise!
			elif [[ $ = scissors ]]
			then
				echo I win: $choise beats paper!
			fi
		;;
		s)
			echo You choose scissors, I choose $choise
			if [[ $choise = scissors ]]
			then
				echo Draw: two scissors!
			elif [[ $choise = paper ]]
			then
				echo You win: scissors beats $choise!
			elif [[ $choise = rock ]]
			then
				echo I win: $choise beats scissors!
			fi
		;;
		q)
			break;
		;;
		quit)
			break;
		;;
		*)
			echo You can choose only between 'r', 'p' and 's'!
		;;
	esac
done
