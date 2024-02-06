#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

struct Player {
    string username;
    vector<int> scores = vector<int>(32,0);
    int points = 0, twoClub = 0, oneClub = 0, daysParticipated = 0, sumOfScores = 0;
};

struct Day{
    int dayNumber = 0, index = 0, winningScore = 6, playersTied = 0;
    vector<Player*> dayParticipants; //scores are stored in eachParticipant->scores.at(index)
};

map<string, Player*> allPlayers; //.at(string) converts strings to pointers of matching name
map<string, Day*> allDays;       //also a quick way to ask if the object exists
                                 //map is technically vector<pair<>>, so I can iterate through it too
                                 //as dumb as this looks, those are 3 things I dont have to program myself
int largestTie = 0; //every program needs one random global variable. Exists so that multiple days with largestTie can be printed

void InputTXT();
void PrintData();
void CalculateWins();
void PrintResults();

int main(){
    InputTXT();
    //PrintData(); //for debugging
    CalculateWins();
    PrintResults();
    system("Pause");
    return 0;
}

void InputTXT(){
    ifstream inFS;
    string line, lastUsername, trash, wordleDay, lastScore;
    inFS.open("scores.txt");
    if(!inFS.is_open()){
        cout << "You need a file called scores.txt in the same directory" << endl;
        cout << "Copy and paste the whole month's Discord chat into that file" << endl;
        system("Pause");
    }
    while(!inFS.eof()){                             //loop through each txt line
        getline(inFS, line);
        if(line.find("—") < line.size()){           //username case
            lastUsername = line;                    //lastUsername == user name — 12/31/23 11:59 PM
            lastUsername.resize(line.find("—")-1);  //lastUsername == user name
        }
        else if(line.find("Wordle") == 0){          //score case. Likely false positive here
            istringstream TXTline(line);
            TXTline >> trash >> wordleDay >> lastScore; //TXTline == Wordle 906 3/6
            if(!allPlayers.count(lastUsername)){ //create player if it doesn't exist
                allPlayers.emplace(lastUsername, new Player{lastUsername});
            }
            if(!allDays.count(wordleDay)){ //create day if it doesn't exist
                allDays.emplace(wordleDay, new Day{stoi(wordleDay), (int)allDays.size()}); //allDays.at(wordleDay)->dayNumber = stoi(wordleDay); allDays.at(wordleDay)->index = allDays.size()-1;
            }
            allDays.at(wordleDay)->dayParticipants.push_back(allPlayers.at(lastUsername)); //link player to day. Day*->dayParticipants.push_back(Player*)
            int& score = allPlayers.at(lastUsername)->scores.at(allDays.at(wordleDay)->index); //score = Player*->scores.at(Day); ugly because of the map<string, pointer> conversion
            if(lastScore.at(0) == 'X'){score = 7;} //extract loss from X/6
            else{score = lastScore.at(0) - '0';} //extract int 3 from string 3/6 
            allPlayers.at(lastUsername)->sumOfScores += score; //to calculate average
            allPlayers.at(lastUsername)->daysParticipated++; //to calculate average
        }
    }
}

void PrintData(){
    for(pair<string, Day*> eachDayPair : allDays){
        Day* eachDay = eachDayPair.second;
        cout << "Day " << eachDay->dayNumber << endl;
        for(Player* eachPlayer : eachDay->dayParticipants){ //for every player who participated
            cout << eachPlayer->username << '\t' << eachPlayer->scores.at(eachDay->index) << endl;
        }
        cout << endl;
    }
    cout << endl;
}

void CalculateWins(){
    for(pair<string, Day*> eachDayPair : allDays){ //for every day
        Day* eachDay = eachDayPair.second;
        for(Player* eachPlayer : eachDay->dayParticipants){ //check all players that day for the lowest score
            if(eachPlayer->scores.at(eachDay->index) < eachDay->winningScore){
                eachDay->winningScore = eachPlayer->scores.at(eachDay->index); //update lowest score
            }
        }
        for(Player* eachPlayer : eachDay->dayParticipants){ //check all players again for winners
            int& playerScore = eachPlayer->scores.at(eachDay->index);
            if(playerScore == eachDay->winningScore){ eachPlayer->points++; eachDay->playersTied++; }
            if(playerScore == 2){ eachPlayer->twoClub++; } // 2/6
            if(playerScore == 1){ eachPlayer->oneClub++; } // 1/6
        }
        if(eachDay->playersTied > largestTie){ //finds the number of players on the largest tie
            largestTie = eachDay->playersTied; //will output multiple days if they have the same tie
        }
    }
}

void PrintResults(){
    cout  << '\t' << '\t' << "Points:" << '\t' << "average" << '\t' << "days played" << endl;
    int totalPoints = 0;
    for(pair<string, Player*> eachPlayerPair : allPlayers){ //sum up all scores
        Player* eachPlayer = eachPlayerPair.second;
        string average = to_string((double)eachPlayer->sumOfScores / eachPlayer->daysParticipated);
        average.resize(4); //for whatever reason setprecision doesnt work
        cout << eachPlayer->username;
        if(eachPlayer->username.size() < 7){cout << '\t';}
        cout << '\t' << '(' << eachPlayer->points << ')' << '\t' << average + "/6" << '\t' << eachPlayer->daysParticipated << endl;
        totalPoints += eachPlayer->points;
    }
    cout << endl << "Month's total points: " << totalPoints << endl;
    cout << endl << "2/6 Club: " << endl;
    for(pair<string, Player*> eachPlayerPair : allPlayers){ //loop through players again to print those who scored 2
        Player* eachPlayer = eachPlayerPair.second;
        if(eachPlayer->twoClub > 0){
            cout << eachPlayer->username;
            if(eachPlayer->username.size() < 7){cout << '\t';}
            cout << '\t' << '(' << eachPlayer->twoClub << ')' << endl;
        }
    }
    cout << endl << "Largest Tie(s): " << endl;
    for(pair<string, Day*> eachDayPair : allDays){ //loop through to find days matching the max tie. If 7 people tie on 2 days, it prints both
        Day* eachDay = eachDayPair.second;
        if(eachDay->playersTied == largestTie){ 
            cout << "Day: " << eachDay->dayNumber << " " << eachDay->winningScore << "/6" << endl;
            for(Player* eachPlayer : eachDay->dayParticipants){ //print all participating players that had the winning score
                if(eachPlayer->scores.at(eachDay->index) == eachDay->winningScore){
                    cout << '\t' << eachPlayer->username << endl;
                }
            }
        }
    }
    for(pair<string, Player*> eachPlayerPair : allPlayers){ //in the unlikely event of 1/6
        Player* eachPlayer = eachPlayerPair.second;
        if(eachPlayer->oneClub > 0){
            cout << endl << "1/6: " << endl << eachPlayer->username << endl;
        }
    }
}