/*Enum of Game results*/
typedef enum {
	eDRAW = -1,
	eLOST,
	eWINN
} decision_t;
/*Enum of Unknown, Rock, Paper and Scissors choise*/
typedef enum {
	eU = -1,
	eR,
	eP,
	eS
} element_t;
const int game_table[3][3]={
       eDRAW, eLOST, eWINN,
       eWINN, eDRAW, eLOST,
       eLOST, eWINN, eDRAW
};

