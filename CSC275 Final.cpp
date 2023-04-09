#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <memory>
using namespace std;

//Cards Classes
class Cards
{
public:
	string symbol;

	virtual void Played()
	{
		cout << "This is base cards" << endl;
	};
protected:
	string id_num;
};

class NormalCards : Cards
{
public:
	string symbol = "R 0";

	string player;

	NormalCards(string s, string p)
	{
		symbol = s;

		player = p;
	}

	void Played()
	{
		cout << player << " played " << symbol << endl;
		//update player cards
	}
};

class SpeacialCards : Cards
{
public:
	string symbol = "R S";

	int type;

	string player;

	void Played()
	{
		cout << player << "played " << symbol << endl;

		switch (type)
		{
			case 0:
			{
				cout << "Played reverse" << endl;
				return;
			}
			case 1:
			{
				cout << "Played skip" << endl;
				return;
			}
			case 2:
			{
				cout << "Played +2" << endl;
				return;
			}
			case 3:
			{
				cout << "Played wild" << endl;
				return;
			}
			case 4:
			{
				cout << "Played +4" << endl;
				return;
			}
		}
		//Update playable cards
	}

private:
	string ability;
};

//Future struct to store new users
struct Users {
	int id;
	string name;
	int wins;
	int loss;
	float ratio;
};

//Struct to save the AI opponents
struct Ai {
	int id;
	string commonName;
	int wins;
	int loss;
	float ratio;
};

//Initialized Function
void Deal(int* startingPlayer, bool* isReversed);

void PlayerTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw);

void DumbTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw);

void SmartTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw);

void TargetTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw);

void DetermineNextPlayer(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw);

void Draw(int& currentPlayerIndex);

string NumToCard(int& cardNum);

int CardToNum(string& card);

void CardsToPlay(int currentCard, vector<int> hand, bool* canJumpIn);

int CardPlayed(int cardPlayed);

void cardEffects(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw, int* appliedEffects);

//Constant Variables for the 4 different players
const string PLAYER = "Player";
const string DUMB = "Dumb";
const string SMART = "Smart";
const string TARGET = "Target";

//The Max and Mininum Index for player Indexes
const int MAX_INDEX = 3;
const int MIN_INDEX = 0;

//Starting player and turn counter
int startingIndex = 3;
int turnCounter = 0;

//Lets the system know the order has been reversed
bool isOrderReversed = false;

string currentColor = "RED";
int currentCard;

//Vector to store cards left in the dedck
vector<int> deck = {
	0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
	10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,19,19,
	20,20,21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29,
	30,30,31,31,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,
	40,40,41,41,42,42,43,43,
	44,44,45,45,46,46,47,47,
	48,48,49,49,50,50,51,51,
	52,52,52,52,53,53,53,53
};

//Vectore to store all cards that have been played
vector<int> discardPile;

//Vector for the player hand
vector<string> playerHand;

//Vector for the dumb ai's hand
vector<string> dumbHand;

//Vector for the smart ai's hand
vector<string> smartHand;

//Vectore for the target ai's hand
vector<string> targetHand;

vector<int> handToPlay;

vector<string> playableCards;

//The order of players
string playerIndexes[4] = {
	"Dumb",
	"Smart",
	"Player",
	"Target"
};

//Main function NOTE this will be cleaned up to act like a better Main in the future. 
int main()
{
	srand(time(0));

	//Some strings to get data from the text documents
	string dumbWin;
	string dumbLoss;
	string dumbRatio;
	string smartWin;
	string smartLoss;
	string smartRatio;
	string targetWin;
	string targetLoss;
	string targetRatio;

	//Input files, or files we read from
	ifstream winfile("winFile.txt");
	ifstream lossfile("lossFile.txt");
	ifstream ratiofile("ratioFile.txt");

	//Opening win stats and assigning values to different ai's
	if (winfile.is_open())
	{
		int i = 1;
		getline(winfile, dumbWin);
		while (i <= 2)
		{
			if (i == 1)
			{
				getline(winfile, smartWin);
			}
			else
			{
				getline(winfile, targetWin);
			}
			i++;
		}

		winfile.close();
	}
	else cout << "Unable to open win file";

	//Opening loss file and assigning values for the ai stats
	if (lossfile.is_open())
	{
		int i = 1;
		getline(lossfile, dumbLoss);
		while (i <= 2)
		{
			if (i == 1)
			{
				getline(lossfile, smartLoss);
			}
			else
			{
				getline(lossfile, targetLoss);
			}
			i++;
		}

		lossfile.close();
	}
	else cout << "Unable to open loss file";

	//Opeining the ratio file and assigning values for the ai's stats
	if (ratiofile.is_open())
	{
		int i = 1;
		getline(ratiofile, dumbRatio);
		while (i <= 2)
		{
			if (i == 1)
			{
				getline(ratiofile, smartRatio);
			}
			else
			{
				getline(ratiofile, targetRatio);
			}
			i++;
		}

		ratiofile.close();
	}
	else cout << "Unable to open ratio file";

	//Creating instances of the ai
	Ai dumb = Ai{};
	Ai smart = Ai{};
	Ai target = Ai{};

	//Dumb ai
	dumb.id = 1;
	dumb.commonName = "Dumb";
	dumb.wins = stoi(dumbWin);
	dumb.loss = stoi(dumbLoss);
	dumb.ratio = stof(dumbRatio);

	//Smart ai
	smart.id = 2;
	smart.commonName = "Smart";
	smart.wins = stoi(smartWin);
	smart.loss = stoi(smartLoss);
	smart.ratio = stof(smartRatio);

	//Targeting ai
	target.id = 3;
	target.commonName = "Target";
	target.wins = stoi(targetWin);
	target.loss = stoi(targetLoss);
	target.ratio = stof(targetRatio);

	//Displaying opponents stats
	cout << "Your opponents today are: " << endl;
	cout << "    " << dumb.commonName << ": " << endl;
	cout << "        Wins: " << dumb.wins << endl;
	cout << "        Losses: " << dumb.loss << endl;
	cout << "        W/L Ratio: " << dumb.ratio << endl;
	cout << "    " << smart.commonName << ": " << endl;
	cout << "        Wins: " << smart.wins << endl;
	cout << "        Losses: " << smart.loss << endl;
	cout << "        W/L Ratio: " << smart.ratio << endl;
	cout << "    " << target.commonName << ": " << endl;
	cout << "        Wins: " << target.wins << endl;
	cout << "        Losses: " << target.loss << endl;
	cout << "        W/L Ratio: " << target.ratio << endl;

	//Calls Deal
	Deal(&startingIndex, &isOrderReversed);

	cout << "Player Hand" << endl;

	for (int i = 0; i < playerHand.size(); i++)
	{
		cout << playerHand[i] << " ";
	}

	cout << endl;

	cout << "Smart Hand" << endl;

	for (int i = 0; i < smartHand.size(); i++)
	{
		cout << smartHand[i] << " ";
	}

	cout << endl;

	cout << "Dumb Hand" << endl;

	for (int i = 0; i < dumbHand.size(); i++)
	{
		cout << dumbHand[i] << " ";
	}

	cout << endl;

	cout << "Target hand" << endl;

	for (int i = 0; i < targetHand.size(); i++)
	{
		cout << targetHand[i] << " ";
	}

	cout << endl;

	//Updates who won and who lost
	dumb.wins++;
	smart.loss++;
	target.loss++;

	//Checking if loss is equal to zero before making the ratio. If so, the ration is set to the amount of wins. 
	if (dumb.loss != 0)
	{
		dumb.ratio = dumb.wins / smart.loss;
	}
	else
	{
		dumb.ratio = dumb.wins;
	}

	//Same thing but for the smart ai
	if (smart.loss != 0)
	{
		smart.ratio = smart.wins / smart.loss;
	}
	else 
	{
		smart.ratio = smart.wins;
	}

	//Same thing but for the targeting ai
	if (target.loss != 0)
	{
		target.ratio = target.wins / target.loss;
	}
	else
	{
		target.ratio = target.wins;
	}

	//Writing to the win file
	ofstream winFile("winFile.txt");
	if (winFile.is_open())
	{
		winFile << dumb.wins << endl;
		winFile << smart.wins << endl;
		winFile << target.wins << endl;
		winFile.close();
	}
	else cout << "Unable to open win file";

	//Writing to the loss file
	ofstream lossFile("lossFile.txt");
	if (lossFile.is_open())
	{
		lossFile << dumb.loss << endl;
		lossFile << smart.loss << endl;
		lossFile << target.loss << endl;
		lossFile.close();
	}
	else cout << "Unable to open loss file";

	//Writing to the ratio file
	ofstream ratioFile("ratioFile.txt");
	if (ratioFile.is_open())
	{
		ratioFile << dumb.ratio << endl;
		ratioFile << smart.ratio << endl;
		ratioFile << target.ratio << endl;
		ratioFile.close();
	}
	else cout << "Unable to open ratio file";
	
	//Return 0
	return 0;
}

//Deals the cards, then starts with the player next to the dealer, or the player after the starting index
void Deal(int* startingPlayer, bool* isReversed)
{
	bool hasWon = false;
	bool hasBeenSkipped = false;
	bool canJumpIn = false;
	int secondPlayer;
	int thirdPlayer;
	int fourthPlayer;
	int zero = 0;
	if (*startingPlayer == MAX_INDEX)
	{
		secondPlayer = *startingPlayer - MAX_INDEX;
		thirdPlayer = *startingPlayer - MAX_INDEX + 1;
		fourthPlayer = *startingPlayer - MAX_INDEX + 2;
	}
	else if (*startingPlayer == MAX_INDEX - 1)
	{
		secondPlayer = *startingPlayer - MAX_INDEX + 1;
		thirdPlayer = *startingPlayer - MAX_INDEX + 2;
		fourthPlayer = *startingPlayer - MAX_INDEX;
	}
	else if (*startingPlayer == MAX_INDEX - 2)
	{
		secondPlayer = *startingPlayer - MAX_INDEX + 2;
		thirdPlayer = *startingPlayer - MAX_INDEX;
		fourthPlayer = *startingPlayer - MAX_INDEX + 1;
	}
	else
	{
		secondPlayer = *startingPlayer + 1;
		thirdPlayer = *startingPlayer + 2;
		fourthPlayer = *startingPlayer + 3;
	}
	for (int i = 0; i <= 6; i++)
	{
		Draw(*startingPlayer);
		Draw(secondPlayer);
		Draw(thirdPlayer);
		Draw(fourthPlayer);
	}
	cout << "I am dealing the cards starting with player " << *startingPlayer << ". Order reversed: " << *isReversed << endl;
	DetermineNextPlayer(isReversed, startingPlayer, &hasWon, &hasBeenSkipped, &canJumpIn, &zero);
}

//The Players turn. As long as the game has not been won, the player will be able to play or draw
void PlayerTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw)
{
	if (!*hasWon)
	{
		int cardPlayed = 0;

		cout << "Current Card: " << NumToCard(currentCard) << endl;

		while (true)
		{
			handToPlay.clear();
			playableCards.clear();

			cout << "Your Hand: " << endl;

			for (int i = 0; i < playerHand.size(); i++)
			{
				cout << playerHand[i] << " ";
				handToPlay.push_back(CardToNum(playerHand[i]));
			}

			cout << endl;

			CardsToPlay(currentCard, handToPlay, canJumpIn);

			if (playableCards.size() != NULL)
			{
				int selectedCard;
				bool legalCardSelected = false;

				for (int i = 0; i < playableCards.size(); i++)
				{
					cout << i + 1 << ": " << playableCards[i] << " " << endl;
				}

				cout << "Select a card to play: ";
				cin >> selectedCard;

				selectedCard -= 1;

				for (int i = 0; i < playableCards.size(); i++)
				{
					if (i == selectedCard)
					{
						legalCardSelected = true;
					}
				}

				if (legalCardSelected)
				{
					cardPlayed = CardPlayed(CardToNum(playableCards[selectedCard]));
					for (int i = 0; i < playerHand.size(); i++)
					{
						if (playableCards[selectedCard] == playerHand[i])
						{
							playerHand.erase(playerHand.begin() + i);
						}
					}
					break;
				}
				else
				{
					cout << "Please select a card that is able to be played." << endl;
				}
			}
			else if (*cardsToDraw > 0)
			{
				cout << "Drawing " << *cardsToDraw << "cards!" << endl;
				for (int i = 0; i < *cardsToDraw; i++)
				{
					Draw(*currentPlayerIndex);
				}
				*cardsToDraw = 0;
				break;
			}
			else
			{
				cout << "Drawing a card" << endl;
				Draw(*currentPlayerIndex);
			}
		}

		turnCounter++;
		cout << "Deleting played cards smart pointer" << endl;

		cardEffects(isReversed, currentPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw, &cardPlayed);
		
	}
	else
	{
		cout << "Game Over" << endl;
	}
}

//This is the dumb ai's turn. It will play the first card it can
void DumbTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw)
{
	if (!*hasWon)
	{
		cout << "This is the dumb turn" << endl;
		turnCounter++;
		DetermineNextPlayer(isReversed, currentPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);
	}
	else
	{
		cout << "Game Over" << endl;
	}
}

//This is the smart ai's turn. It will iterate through every card and then pick the one that will be the most useful
void SmartTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw)
{
	if (!*hasWon)
	{
		cout << "This is the smart turn" << endl;
		turnCounter++;
		DetermineNextPlayer(isReversed, currentPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);
	}
	else
	{
		cout << "Game Over" << endl;
	}
}

//This is the targeting ai's turn. It will pick speacial cards first and then play more like dumb
void TargetTurn(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw)
{
	if (!*hasWon)
	{
		cout << "This is the targeting turn" << endl;
		turnCounter++;
		DetermineNextPlayer(isReversed, currentPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);
	}
	else
	{
		cout << "Game Over" << endl;
	}
}

//This is the function to determine the next player
void DetermineNextPlayer(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw)
{
	//Sets some temporary values within the function
	int tempMinIndex = MIN_INDEX;
	int tempMaxIndex = MAX_INDEX;
	int tempPlayerIndex;

	//Turn counter not permanent
	if (turnCounter > 15)
	{
		*hasWon = true;
	}

	//If the order is not reversed, then it will move to the player with the next index
	if (!*isReversed && !*isSkipped)
	{
		//Setting what the next index should be
		if (*currentPlayerIndex >= MAX_INDEX)
		{
			tempPlayerIndex = MIN_INDEX;
		}
		else
		{
			tempPlayerIndex = *currentPlayerIndex + 1;
		}
	}
	//If it is Reversed, it goes to the index one lower than the current index
	else if(*isReversed && !*isSkipped)
	{
		if (*currentPlayerIndex <= MIN_INDEX)
		{
			tempPlayerIndex = MAX_INDEX;
		}
		else
		{
			tempPlayerIndex = *currentPlayerIndex - 1;
		}
	}
	else if (!*isReversed && *isSkipped)
	{
		//Setting what the next index should be
		if (*currentPlayerIndex == MAX_INDEX - 1)
		{
			tempPlayerIndex = MIN_INDEX;
		}
		else if (*currentPlayerIndex >= MAX_INDEX)
		{
			tempPlayerIndex = MIN_INDEX + 1;
		}
		else
		{
			tempPlayerIndex = *currentPlayerIndex + 2;
		}

		*isSkipped = false; 

	}
	else if (*isReversed && *isSkipped)
	{
		if (*currentPlayerIndex == MIN_INDEX + 1)
		{
			tempPlayerIndex = MAX_INDEX;
		}
		else if (*currentPlayerIndex <= MIN_INDEX)
		{
			tempPlayerIndex = MAX_INDEX - 1;
		}
		else
		{
			tempPlayerIndex = *currentPlayerIndex - 2;
		}

		*isSkipped = false;

	}

	if (playerIndexes[tempPlayerIndex] == DUMB)
	{
		DumbTurn(isReversed, &tempPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);
	}
	else if (playerIndexes[tempPlayerIndex] == SMART)
	{

		SmartTurn(isReversed, &tempPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);

	}
	else if (playerIndexes[tempPlayerIndex] == TARGET)
	{

		TargetTurn(isReversed, &tempPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);

	}
	else if (playerIndexes[tempPlayerIndex] == PLAYER)
	{

		PlayerTurn(isReversed, &tempPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);

	}
}

//This is the function that draws cards for players
void Draw(int& currentPlayerIndex)
{
	int cardIndex;
	int cardNum; //Random card index from the array of ints above known as deck;

	cardIndex = (rand() % deck.size());

	cardNum = deck[cardIndex];

	cout << cardIndex << " " << cardNum << endl;

	deck.erase(deck.begin() + cardIndex);

	if (playerIndexes[currentPlayerIndex] == PLAYER)
	{
		playerHand.push_back(NumToCard(cardNum));
	}
	else if (playerIndexes[currentPlayerIndex] == DUMB)
	{
		dumbHand.push_back(NumToCard(cardNum));
	}
	else if (playerIndexes[currentPlayerIndex] == SMART)
	{
		smartHand.push_back(NumToCard(cardNum));
	}
	else if (playerIndexes[currentPlayerIndex] == TARGET)
	{
		targetHand.push_back(NumToCard(cardNum));
	}
}

//This switches cards from ID numbers to cards that can be shown to players
string NumToCard(int& cardNum) 
{
	switch (cardNum)
	{
		case 0:
		{
			return "R 0";
		}
		case 1:
		{
			return "R 1";
		}
		case 2:
		{
			return "R 2";
		}
		case 3:
		{
			return "R 3";
		}
		case 4:
		{
			return "R 4";
		}
		case 5:
		{
			return "R 5";
		}
		case 6:
		{
			return "R 6";
		}
		case 7:
		{
			return "R 7";
		}
		case 8:
		{
			return "R 8";
		}
		case 9:
		{
			return "R 9";
		}
		case 10:
		{
			return "B 0";
		}
		case 11:
		{
			return "B 1";
		}
		case 12:
		{
			return "B 2";
		}
		case 13:
		{
			return "B 3";
		}
		case 14:
		{
			return "B 4";
		}
		case 15:
		{
			return "B 5";
		}
		case 16:
		{
			return "B 6";
		}
		case 17:
		{
			return "B 7";
		}
		case 18:
		{
			return "B 8";
		}
		case 19:
		{
			return "B 9";
		}
		case 20:
		{
			return "G 0";
		}
		case 21:
		{
			return "G 1";
		}
		case 22:
		{
			return "G 2";
		}
		case 23:
		{
			return "G 3";
		}
		case 24:
		{
			return "G 4";
		}
		case 25:
		{
			return "G 5";
		}
		case 26:
		{
			return "G 6";
		}
		case 27:
		{
			return "G 7";
		}
		case 28:
		{
			return "G 8";
		}
		case 29:
		{
			return "G 9";
		}
		case 30:
		{
			return "Y 0";
		}
		case 31:
		{
			return "Y 1";
		}
		case 32:
		{
			return "Y 2";
		}
		case 33:
		{
			return "Y 3";
		}
		case 34:
		{
			return "Y 4";
		}
		case 35:
		{
			return "Y 5";
		}
		case 36:
		{
			return "Y 6";
		}
		case 37:
		{
			return "Y 7";
		}
		case 38:
		{
			return "Y 8";
		}
		case 39:
		{
			return "Y 9";
		}
		case 40:
		{
			return "R S";
		}
		case 41:
		{
			return "B S";
		}
		case 42:
		{
			return "G S";
		}
		case 43:
		{
			return "Y S";
		}
		case 44:
		{
			return "R R";
		}
		case 45:
		{
			return "B R";
		}
		case 46:
		{
			return "G R";
		}
		case 47:
		{
			return "Y R";
		}
		case 48:
		{
			return "R+2";
		}
		case 49:
		{
			return "B+2";
		}
		case 50:
		{
			return "G+2";
		}
		case 51:
		{
			return "Y+2";
		}
		case 52:
		{
			return "WILD";
		}
		case 53:
		{
			return "W+4";
		}
	}
}

//This does the opposite of the one above
int CardToNum(string& card)
{
	if (card == "R 0")
	{
		return 0;
	}
	else if (card == "R 1")
	{
		return 1;
	}
	else if (card == "R 2")
	{
		return 2;
	}
	else if (card == "R 3")
	{
		return 3;
	}
	else if (card == "R 4")
	{
		return 4;
	}
	else if (card == "R 5")
	{
		return 5;
	}
	else if (card == "R 6")
	{
		return 6;
	}
	else if (card == "R 7 ")
	{
		return 7;
	}
	else if (card == "R 8")
	{
		return 8;
	}
	else if (card == "R 9")
	{
		return 9;
	}
	else if (card == "B 0")
	{
		return 10;
	}
	else if (card == "B 1")
	{
		return 11;
	}
	else if (card == "B 2")
	{
		return 12;
	}
	else if (card == "B 3")
	{
		return 13;
	}
	else if (card == "B 4")
	{
		return 14;
	}
	else if (card == "B 5")
	{
		return 15;
	}
	else if (card == "B 6")
	{
		return 16;
	}
	else if (card == "B 7 ")
	{
		return 17;
	}
	else if (card == "B 8")
	{
		return 18;
	}
	else if (card == "B 9")
	{
		return 19;
	}
	else if (card == "G 0")
	{
		return 20;
	}
	else if (card == "G 1")
	{
		return 21;
	}
	else if (card == "G 2")
	{
		return 22;
	}
	else if (card == "G 3")
	{
		return 23;
	}
	else if (card == "G 4")
	{
		return 24;
	}
	else if (card == "G 5")
	{
		return 25;
	}
	else if (card == "G 6")
	{
		return 26;
	}
	else if (card == "G 7")
	{
		return 27;
	}
	else if (card == "G 8")
	{
		return 28;
	}
	else if (card == "G 9")
	{
		return 29;
	}
	else if (card == "Y 0")
	{
		return 30;
	}
	else if (card == "Y 1")
	{
		return 31;
	}
	else if (card == "Y 2")
	{
		return 32;
	}
	else if (card == "Y 3")
	{
		return 33;
	}
	else if (card == "Y 4")
	{
		return 34;
	}
	else if (card == "Y 5")
	{
		return 35;
	}
	else if (card == "Y 6")
	{
		return 36;
	}
	else if (card == "Y 7")
	{
		return 37;
	}
	else if (card == "Y 8")
	{
		return 38;
	}
	else if (card == "Y 9")
	{
		return 39;
	}
	else if (card == "R S")
	{
		return 40;
	}
	else if (card == "B S")
	{
		return 41;
	}
	else if (card == "G S")
	{
		return 42;
	}
	else if (card == "Y S")
	{
		return 43;
	}
	else if (card == "R R")
	{
		return 44;
	}
	else if (card == "B R")
	{
		return 45;
	}
	else if (card == "G R")
	{
		return 46;
	}
	else if (card == "Y R")
	{
		return 47;
	}
	else if (card == "R+2")
	{
		return 48;
	}
	else if (card == "B+2")
	{
		return 49;
	}
	else if (card == "G+2")
	{
		return 50;
	}
	else if (card == "Y+2")
	{
		return 51;
	}
	else if (card == "WILD")
	{
		return 52;
	}
	else if (card == "W+4")
	{
		return 53;
	}
}

//Goes through the players hand and tells them what cards can be played
void CardsToPlay(int currentCard, vector<int> hand, bool* canJumpIn)
{
	if (!*canJumpIn)
	{
		if (currentColor == "RED")
		{
			if (currentCard < 40)
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 10 && hand[i] >= 0 || hand[i] == currentCard + 10 || hand[i] == currentCard + 20 || hand[i] == currentCard + 30 || hand[i] == 40 || hand[i] == 44 || hand[i] == 48 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
			else
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 10 && hand[i] >= 0 || hand[i] == 40 || hand[i] == 44 || hand[i] == 48 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
		}
		else if (currentColor == "BLUE")
		{
			if (currentCard < 40)
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 20 && hand[i] >= 10 || hand[i] == currentCard - 10 || hand[i] == currentCard + 10 || hand[i] == currentCard + 20 || hand[i] == 41 || hand[i] == 45 || hand[i] == 49 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
			else
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 20 && hand[i] >= 10 || hand[i] == 41 || hand[i] == 45 || hand[i] == 49 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
		}
		else if (currentColor == "GREEN")
		{
			if (currentCard < 40)
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 30 && hand[i] >= 20 || hand[i] == currentCard - 20 || hand[i] == currentCard - 10 || hand[i] == currentCard + 10 || hand[i] == 42 || hand[i] == 46 || hand[i] == 50 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
			else
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 30 && hand[i] >= 10 || hand[i] == 42 || hand[i] == 46 || hand[i] == 50 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
		}
		else if (currentColor == "YELLOW")
		{
			if (currentCard < 40)
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 40 && hand[i] >= 30 || hand[i] == currentCard - 30 || hand[i] == currentCard - 20 || hand[i] == currentCard - 10 || hand[i] == 43 || hand[i] == 47 || hand[i] == 51 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
			else
			{
				for (int i = 0; i < hand.size(); i++)
				{
					if (hand[i] < 40 && hand[i] >= 30 || hand[i] == 43 || hand[i] == 47 || hand[i] == 51 || hand[i] == 52 || hand[i] == 53)
					{
						playableCards.push_back(NumToCard(hand[i]));
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < hand.size(); i++)
		{
			if (hand[i] >= 48 && hand[i] < 52 || hand[i] == 53)
			{
				playableCards.push_back(NumToCard(hand[i]));
			}
		}
	}
}

int CardPlayed(int cardPlayed)
{
	discardPile.push_back(cardPlayed);
	currentCard = cardPlayed;
	if (cardPlayed < 40)
	{
		if (cardPlayed < 10 && cardPlayed >= 0)
		{
			currentColor = "RED";
		}
		else if (cardPlayed < 20 && cardPlayed >= 10) 
		{
			currentColor = "BLUE";
		}
		else if (cardPlayed < 30 && cardPlayed >= 20)
		{
			currentColor = "GREEN";
		}
		else if (cardPlayed < 40 && cardPlayed >= 30)
		{
			currentColor = "YELLOW";
		}
		return 0;
	}
	else if(cardPlayed >= 40 && cardPlayed < 52)
	{
		if (cardPlayed < 44)
		{
			if (cardPlayed == 40)
			{
				currentColor = "RED";
			}
			else if (cardPlayed == 41)
			{
				currentColor = "BLUE";
			}
			else if (cardPlayed == 42)
			{
				currentColor = "GREEN";
			}
			else if (cardPlayed == 43)
			{
				currentColor = "YELLOW";
			}
			return 1;
		}
		else if (cardPlayed < 48)
		{
			if (cardPlayed == 44)
			{
				currentColor = "RED";
			}
			else if (cardPlayed == 45)
			{
				currentColor = "BLUE";
			}
			else if (cardPlayed == 46)
			{
				currentColor = "GREEN";
			}
			else if (cardPlayed == 47)
			{
				currentColor = "YELLOW";
			}
			return 2;
		}
		else if (cardPlayed < 52)
		{
			if (cardPlayed == 48)
			{
				currentColor = "RED";
			}
			else if (cardPlayed == 49)
			{
				currentColor = "BLUE";
			}
			else if (cardPlayed == 50)
			{
				currentColor = "GREEN";
			}
			else if (cardPlayed == 51)
			{
				currentColor = "YELLOW";
			}
			return 3;
		}
	}
	else
	{
		if (cardPlayed == 52)
		{
			return 4;
		}
		else
		{
			return 5;
		}
	}
}

void cardEffects(bool* isReversed, int* currentPlayerIndex, bool* hasWon, bool* isSkipped, bool* canJumpIn, int* cardsToDraw, int* appliedEffects)
{
	if (*appliedEffects == 1)
	{
		*isSkipped = true;
	}
	else if (*appliedEffects == 2)
	{
		*isReversed = !*isReversed;
	}
	else if (*appliedEffects == 3)
	{
		*canJumpIn = true;
		*cardsToDraw += 2;
	}
	else if (*appliedEffects == 4 || *appliedEffects == 5)
	{
		if (*appliedEffects == 5)
		{
			*canJumpIn = true;
			*cardsToDraw += 4;
		}
		if (playerIndexes[*currentPlayerIndex] == "Player")
		{
			while (true)
			{
				cin.ignore();
				string colorChoice;				
				cout << "Which color would you like to change it too?" << endl;
				cout << "1: Red, 2: Blue, 3: Green, 4: Yellow ";
				cin >> colorChoice;
				if (colorChoice == "1")
				{
					currentColor = "RED";
					break;
				}
				else if (colorChoice == "2")
				{
					currentColor = "BLUE";
					break;
				}
				else if (colorChoice == "3")
				{
					currentColor = "GREEN";
					break;
				}
				else if (colorChoice == "4")
				{
					currentColor = "YELLOW";
					break;
				}
				else
				{
					cout << "Please type 1, 2, 3, or 4." << endl;
				}
			}
		}
		else 
		{
			int colorChoice = (rand() % 4) + 1;
			if (colorChoice == 1)
			{
				currentColor = "RED";
			}
			else if (colorChoice == 2)
			{
				currentColor = "BLUE";
			}
			else if (colorChoice == 3)
			{
				currentColor = "GREEN";
			}
			else if (colorChoice == 4)
			{
				currentColor = "YELLOW";
			}
		}
	}

	DetermineNextPlayer(isReversed, currentPlayerIndex, hasWon, isSkipped, canJumpIn, cardsToDraw);
}


/*
Deal(int* startingPlayer, bool* isReversed)

PlayerTurn(bool* isReversed, string* nextPlayer)
-> <-
DumbTurn(bool* isReversed, string* nextPlayer)
-> <-
SmartTurn(bool* isReversed, string* nextPlayer)
-> <-
TargetTurn(bool* isReversed, string* nextPlayer)
-> <-

-> turn
<-reverse turn

uno()



*/

/*
TO DO:
Play cards
	Keep track of playable cards
	Skips
		Turn is skipped on and off
	Reverse
		Turn is reversed on
	+2
		Turn is skipped on and off unless player has a +2
		Draw 2 cards
	+4 
		Turn is skipped on and off unless player has a +4
		Draw 4 cards
		Change playable cards
Uno
	who says uno when
	Can win if uno was said
Shuffle
	Shuffles discard back into deck

*/

/*
0-9 = Red 
	0 = 0
	1 = 1
	2 = 2
	3 = 3
	4 = 4
	5 = 5
	6 = 6
	7 = 7
	8 = 8
	9 = 9

10-19 = Blue
	10 = 0
	11 = 1
	12 = 2
	13 = 3
	14 = 4
	15 = 5
	16 = 6
	17 = 7
	18 = 8
	19 = 9

20-29 = Green
	20 = 0
	21 = 1
	22 = 2
	23 = 3
	24 = 4
	25 = 5
	26 = 6
	27 = 7
	28 = 8
	29 = 9

30-39 = Yellow
	30 = 0
	31 = 1
	32 = 2
	33 = 3
	34 = 4
	35 = 5
	36 = 6
	37 = 7
	38 = 8
	39 = 9

40 - 53Special Cards
	40 = Red Skip
	41 = Blue Skip
	42 = Green Skip
	43 = Yellow Skip

	44 = Red Reverse
	45 = Blue Reverse
	46 = Green Reverse
	47 = Yellow Reverse

	48 = Red Draw
	49 = Blue Draw
	50 = Green Draw
	51 = Yellow Draw

	52 = Wild
	53 = Draw 4




*/



//Uno


/*
4 Colors
+2
+4
Switch Colors
Reverse
Skip

Deck
Deal 7 Cards
Draw Cards
Play Cards

4 Players

Ai:
Dumb
//Plays First Card that is able
//Never says Uno
Smart
//Goes through every card and plays the best card
//Always says uno
Targets Player
//Similar to smart, but if it can play a +2, skip, or +4, it will

*/
