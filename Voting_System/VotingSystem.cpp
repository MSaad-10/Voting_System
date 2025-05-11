#include <iostream>
#include <string>
#include <mysql.h>
#include <algorithm> // for transform
#include <cctype>  // for tolower
#include "Validation.h"
using namespace std;

MYSQL* conn;
MYSQL_RES* res;
MYSQL_ROW row;

// No Changes required in this section as this is just for connecting the C++ to Mysql

void connectDatabase() {
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "mysql01", "voting_system", 3306, NULL, 0);
    if (conn) cout << "Database Connected Successfully!\n";
    else {
        cout << "Database Connection Failed: " << mysql_error(conn) << endl;
        exit(1);
    }
}

// =====================================================================================================================================

// This class is used to check whether the election has started or not yet 

class VotingControl {
public:
    static bool isVotingOpen() {
        mysql_query(conn, "SELECT is_open FROM VotingStatus WHERE id=1");
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        return atoi(row[0]);
    }
    static void setVotingStatus(bool open) {
        string query = "UPDATE VotingStatus SET is_open=" + to_string(open ? 1 : 0) + " WHERE id=1";
        mysql_query(conn, query.c_str());
        cout << (open ? "Voting started.\n" : "Voting closed.\n");
    }
};

// =====================================================================================================================================

// USER class (The parent class)

class User {
protected:
    string username, password, role;
public:
    virtual ~User() {}
    virtual bool login() = 0;
    virtual void menu() = 0;
};

// =====================================================================================================================================

// VOTER class

class Voter : public User {
public:
    bool login() override {
        username = Validation::getNonEmptyString("Username: ");
        password = Validation::getNonEmptyString("Password: ");
        string query = "SELECT role FROM Voters WHERE username='" + username + "' AND password='" + password + "' AND role='voter'";
        mysql_query(conn, query.c_str());
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0)
        {
            return true;
        }
        return false;
    }

    void castToMNA(string userCity) {
        cout << "\nMNA Candidates in your city (" << userCity << "):\n";


        // List candidates from user's city
        string query = "SELECT id, party FROM Candidates WHERE city='" + userCity + "' AND title='MNA'";
        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error fetching candidates: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);

        if (!res || mysql_num_rows(res) == 0) {
            cout << "No candidates available in your city.\n";
            mysql_free_result(res);
            return;
        }

        while ((row = mysql_fetch_row(res))) {
            cout << "Candidate ID: " << row[0] << " | Party: " << row[1] << endl;
        }
        mysql_free_result(res);


        // Now voting
        int voteId;
        cout << "\nEnter candidate ID to vote for: ";
        cin >> voteId;

        // Check if already voted for MNA
        string checkQuery = "SELECT * FROM Votes WHERE username='" + username + "' AND vote_type='MNA'";
        if (mysql_query(conn, checkQuery.c_str()) != 0) {
            cout << "Error checking existing vote: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) > 0) {
            cout << "You have already voted for MNA!\n";
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);

        // Insert vote
        string voteQuery = "INSERT INTO Votes (username, candidate_id, vote_type) VALUES ('" + username + "', " + to_string(voteId) + ", 'MNA')";
        if (mysql_query(conn, voteQuery.c_str()) != 0) {
            cout << "Error casting vote: " << mysql_error(conn) << endl;
            return;
        }

        // Mark MNA_vote as true
        string updateVoterQuery = "UPDATE Voters SET MNA_vote=1 WHERE username='" + username + "'";
        mysql_query(conn, updateVoterQuery.c_str());
        cout << "Vote cast successfully! Thank you for voting.\n";

        // After updating MNA_vote or MPA_vote
        string statusCheck = "SELECT MNA_vote, MPA_vote FROM Voters WHERE username='" + username + "'";
        mysql_query(conn, statusCheck.c_str());
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if (row && strcmp(row[0], "1") == 0 && strcmp(row[1], "1") == 0) {
            string updateStatus = "UPDATE Voters SET vote_status=1 WHERE username='" + username + "'";
            mysql_query(conn, updateStatus.c_str());
        }
        mysql_free_result(res);

        // Update votes attribute of this candidate in Candidates table
        string updateVotesQuery = "UPDATE Candidates SET votes = votes + 1 WHERE id=" + to_string(voteId);
        mysql_query(conn, updateVotesQuery.c_str());
        cout << "Vote updated successfully!\n";


        // Total Votes Casted to this Candidate so far 
        string totalVotesQuery = "SELECT votes FROM Candidates WHERE id=" + to_string(voteId);
        mysql_query(conn, totalVotesQuery.c_str());
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        int totalVotes = atoi(row[0]);

        cout << "Total Votes Casted to this Candidate: " << totalVotes << endl;
        mysql_free_result(res);

    }
    void castToMPA(string userCity) {
        cout << "\nMPA Candidates in your city (" << userCity << "):\n";
        // List candidates from user's city
        string query = "SELECT id, party FROM Candidates WHERE city='" + userCity + "' AND title='MPA'";
        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error fetching candidates: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);

        if (!res || mysql_num_rows(res) == 0) {
            cout << "No candidates available in your city.\n";
            mysql_free_result(res);
            return;
        }

        while ((row = mysql_fetch_row(res))) {
            cout << "Candidate ID: " << row[0] << " | Party: " << row[1] << endl;
        }
        mysql_free_result(res);
        // Now voting
        int voteId;
        cout << "\nEnter candidate ID to vote for: ";
        cin >> voteId;
        // Check if already voted for MPA
        string checkQuery = "SELECT * FROM Votes WHERE username='" + username + "' AND vote_type='MPA'";
        if (mysql_query(conn, checkQuery.c_str()) != 0) {
            cout << "Error checking existing vote: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) > 0) {
            cout << "You have already voted for MPA!\n";
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);

        // Insert vote
        string voteQuery = "INSERT INTO Votes (username, candidate_id, vote_type) VALUES ('" + username + "', " + to_string(voteId) + ", 'MPA')";
        if (mysql_query(conn, voteQuery.c_str()) != 0) {
            cout << "Error casting vote: " << mysql_error(conn) << endl;
            return;
        }
        // Mark MPA_vote as true
        string updateVoterQuery = "UPDATE Voters SET MPA_vote=1 WHERE username='" + username + "'";
        mysql_query(conn, updateVoterQuery.c_str());
        cout << "Vote cast successfully! Thank you for voting.\n";

        // After updating MNA_vote or MPA_vote
        string statusCheck = "SELECT MNA_vote, MPA_vote FROM Voters WHERE username='" + username + "'";
        mysql_query(conn, statusCheck.c_str());
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if (row && strcmp(row[0], "1") == 0 && strcmp(row[1], "1") == 0) {
            string updateStatus = "UPDATE Voters SET vote_status=1 WHERE username='" + username + "'";
            mysql_query(conn, updateStatus.c_str());
        }
        mysql_free_result(res);

        // Update votes attribute of this candidate in Candidates table
        string updateVotesQuery = "UPDATE Candidates SET votes = votes + 1 WHERE id=" + to_string(voteId);
        mysql_query(conn, updateVotesQuery.c_str());
        cout << "Vote updated successfully!\n";


        // Total Votes Casted to this Candidate so far 
        string totalVotesQuery = "SELECT votes FROM Candidates WHERE id=" + to_string(voteId);
        mysql_query(conn, totalVotesQuery.c_str());
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        int totalVotes = atoi(row[0]);

        cout << "Total Votes Casted to this Candidate: " << totalVotes << endl;
        mysql_free_result(res);

    }

    void castVote() {
        if (!VotingControl::isVotingOpen()) {
            cout << "Voting is currently closed.\n";
            return;
        }

        // Get the voter's city
        string query = "SELECT city FROM Voters WHERE username='" + username + "'";
        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error fetching city: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);
        if (!res || mysql_num_rows(res) == 0) {
            cout << "Could not find your city. Please contact admin.\n";
            mysql_free_result(res);
            return;
        }

        row = mysql_fetch_row(res);
        string userCity = row[0];
        mysql_free_result(res); // Free after city fetch

        int choice;
        do {
            cout << "\nCast Vote to: \n1. MNA\n2. MPA\n3. Exit \n";
            choice = Validation::getValidatedInt("Choice: ", 1, 3);
            switch (choice) {
            case 1: castToMNA(userCity);
                break;
            case 2: castToMPA(userCity);
                break;
            }
        } while (choice != 3);

    }

    void menu() override {
        int op;
        do {
            cout << "\nVoter Menu\n1. Cast Vote\n2. Logout\n";
            op = Validation::getValidatedInt("Choice: ", 1, 2);
            switch (op) {
            case 1: castVote();
                break;
            }
        } while (op != 2);
    }
};

// =====================================================================================================================================


// CANDIDATE class

class Candidate : public User {
public:

    bool login() override {
        username = Validation::getNonEmptyString("Username: ");
        password = Validation::getNonEmptyString("Password: ");
        string query = "SELECT id FROM Candidates WHERE username='" + username + "' AND password='" + password + "'";
        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error: " << mysql_error(conn) << endl;
            return false;
        }
        res = mysql_store_result(conn);
        bool success = (mysql_num_rows(res) > 0);
        mysql_free_result(res); // Free the result to avoid memory leaks
        return success;
    }

    void getVoteCount() {
        string query = "SELECT votes FROM Candidates WHERE username='" + username + "' AND password='" + password + "'";
        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error fetching candidate info: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) == 0) {
            cout << "No candidate found with your credentials.\n";
            mysql_free_result(res);
            return;
        }
        row = mysql_fetch_row(res);
        cout << "Total Votes: " << row[0] << endl;
        mysql_free_result(res);

    }
    void getCandidateInfo() {
        string query = "SELECT id, username, party, city, votes, title FROM Candidates WHERE username='" + username + "' AND password='" + password + "'";
        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error fetching candidate info: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) == 0) {
            cout << "No candidate found with your credentials.\n";
            mysql_free_result(res);
            return;
        }
        row = mysql_fetch_row(res);
        cout << "\nYour Candidate Information:\n";
        cout << "ID: " << row[0] << ", Username: " << row[1] << ", Party: " << row[2] << ", City: " << row[3] << ", Title: " << row[5] << endl;
        mysql_free_result(res);
    }

    void menu() override {
        int op;
        do {
            cout << "\nCandidate Menu\n1. Get Total Votes\n2. Candidate Information\n3. Exit\n";
            op = Validation::getValidatedInt("Choice: ", 1, 3);
            switch (op) {
            case 1: getVoteCount(); break;
            case 2: getCandidateInfo(); break;
            case 3: break;
            default: cout << "Invalid choice. Try again.\n";
            }
        } while (op != 3); // Exit on option 3
    }
};

// =====================================================================================================================================

// ADMIN class

class Admin : public User {
public:
    bool login() override {
        username = Validation::getNonEmptyString("Username: ");
        password = Validation::getNonEmptyString("Password: ");
        string query = "SELECT role FROM Users WHERE username='" + username + "' AND password='" + password + "' AND role='admin'";
        mysql_query(conn, query.c_str());
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0) return true;
        return false;
    }

    void addCandidate() {
        string party, city, title;

        party = Validation::getNonEmptyString("Enter party name: ");
        city = Validation::getNonEmptyString("Enter city name: ");
        title = Validation::getNonEmptyString("Enter title (MNA/MPA): ");
        string query = "INSERT INTO Candidates (party, city, title) VALUES ('" + party + "', '" + city + "', '" + title + "')";
        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error adding new candidate: " << mysql_error(conn) << endl;
            return;
        }
        cout << "Candidate added successfully!\n";

        // Show the updated list of candidates
        string Newquery = "SELECT username, password, city, party, title FROM Candidates";
        mysql_query(conn, Newquery.c_str());
        res = mysql_store_result(conn);
        cout << "\nUpdated Candidates List:\n";
        while ((row = mysql_fetch_row(res))) {
            cout << "Username: " << row[0] << ", Password: " << row[1] << ", City: " << row[2] << ", Party: " << row[3] << ", Title: " << row[4] << endl;
        }
        mysql_free_result(res); // Free after showing candidates

    }

    void removeCandidate() {
        string Newquery = "SELECT id, city, party, title FROM Candidates";
        mysql_query(conn, Newquery.c_str());
        res = mysql_store_result(conn);
        cout << "\nCandidates List:\n";
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            cout << "ID: " << row[0] << ", City: " << row[1] << ", Party: " << row[2] << ", Title: " << row[3] << endl;
            count++;
        }
        mysql_free_result(res); // Free after showing candidates

        int id = Validation::getValidatedInt("Enter candidate ID to remove : ", 1, count);
        string query = "DELETE FROM Candidates WHERE id=" + to_string(id);
        mysql_query(conn, query.c_str());
        cout << "Candidate removed successfully!\n";
    }

    void removeVoter() {
        string Newquery = "SELECT id, username, city FROM Voters";
        mysql_query(conn, Newquery.c_str());
        res = mysql_store_result(conn);
        cout << "\nVoters List:\n";
        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            cout << "ID: " << row[0] << ", Username: " << row[1] << ", City: " << row[2] << endl;
            count++;
        }
        mysql_free_result(res); // Free after showing voters

        int id = Validation::getValidatedInt("Enter voter ID to remove : ", 1, count);
        string query = "DELETE FROM Voters WHERE id=" + to_string(id);
        mysql_query(conn, query.c_str());
        cout << "Voter removed successfully!\n";
    }

    void cityResult() {
        // Fetch cities
        string Cityquery = "SELECT name FROM Cities";
        if (mysql_query(conn, Cityquery.c_str()) != 0) {
            cout << "Error fetching cities: " << mysql_error(conn) << endl;
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (!res) {
            cout << "Error storing result: " << mysql_error(conn) << endl;
            return;
        }

        cout << "\nAvailable Cities:\n";
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            cout << row[0] << endl;
        }
        mysql_free_result(res); // Free after showing cities

        string city = Validation::getNonEmptyString("Enter City Name: ");
        string checkCityQuery = "SELECT * FROM Cities WHERE name= LOWER('" + city + "') ";
        if (mysql_query(conn, checkCityQuery.c_str()) != 0) {
            cout << "Error validating city: " << mysql_error(conn) << endl;
            return;
        }

        res = mysql_store_result(conn);
        if (!res) {
            cout << "Error storing result: " << mysql_error(conn) << endl;
            return;
        }

        if (mysql_num_rows(res) == 0) {
            cout << "Invalid city. Please try again.\n";
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res); // Free after checking province

        // Checking candidates' votes in the selected city
        string Candidatequery = "SELECT username, party, title, votes FROM Candidates WHERE city='" + city + "'";
        if (mysql_query(conn, Candidatequery.c_str()) != 0) {
            cout << "Error fetching candidates: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);
        if (!res) {
            cout << "Error storing result: " << mysql_error(conn) << endl;
            return;
        }
        cout << "\nCandidates in " << city << ":\n";
        while ((row = mysql_fetch_row(res))) {
            cout << "Username: " << row[0] << ", Party: " << row[1] << ", Title: " << row[2] << ", Votes: " << row[3] << endl;
        }
        mysql_free_result(res); // Free after showing candidates

        string totalVotesQuery = "SELECT SUM(votes) FROM Candidates WHERE city='" + city + "'";
        if (mysql_query(conn, totalVotesQuery.c_str()) != 0) {
            cout << "Error fetching total votes: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);
        if (!res) {
            cout << "Error storing result: " << mysql_error(conn) << endl;
            return;
        }
        row = mysql_fetch_row(res);
        int totalVotes = atoi(row[0]);
        cout << "Total Votes Casted in " << city << ": " << totalVotes << endl;
        mysql_free_result(res); // Free after showing total votes

        // now we have to show which party's MNA and which perty's MPA has won

        string Maxquery =
            "SELECT party, title, votes FROM Candidates c "
            "WHERE city = '" + city + "' AND "
            "votes = (SELECT MAX(votes) FROM Candidates WHERE city = '" + city + "' AND title = c.title) "
            "AND votes > 0";


        if (mysql_query(conn, Maxquery.c_str()) != 0) {
            cout << "Error fetching winning candidates: " << mysql_error(conn) << endl;
            return;
        }
        res = mysql_store_result(conn);
        if (!res) {
            cout << "Error storing result: " << mysql_error(conn) << endl;
            return;
        }

        cout << "\nWinning Party in " << city << ":\n";
        while ((row = mysql_fetch_row(res))) {
            cout << "Party: " << row[0] << ", Title : " << row[1] << ", Votes : " << row[2] << endl;
        }


    }

    void allResults() {
        cout << "\nVoting Results:\n";
        string query = "SELECT id, city, party, votes FROM Candidates";
        mysql_query(conn, query.c_str());
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res))) {
            cout << "Candidate ID: " << row[0] << ", City: " << row[1] << ", Party: " << row[2] << ", Votes: " << row[3] << endl;
        }
        mysql_free_result(res); // Free after?showing?results
    }

    void menu() override {
        int op;
        do {
            cout << "\nAdmin Menu\n1. Start Voting\n2. Stop Voting\n3. Add Candidate\n4. Remove Candidate\n5. Remove Voter\n6. View Results\n7. Logout\n";
            op = Validation::getValidatedInt("Choice: ", 1, 7);
            switch (op) {
            case 1: VotingControl::setVotingStatus(true); break;
            case 2: VotingControl::setVotingStatus(false); break;
            case 3: addCandidate(); break;
            case 4: removeCandidate(); break;
            case 5: removeVoter(); break;
            case 6: viewResults(); break;
            }
        } while (op != 7);
    }

    void Votesmenu() {
        int op;
        do {
            cout << "\nView Results on:\n1. City Level\n2. Overall\n3. Exit\n";
            op = Validation::getValidatedInt("Choice: ", 1, 3);
            switch (op) {
            case 1: cityResult(); break;
            case 2:; allResults(); break;
            case 3: break;
            default: cout << "Invalid choice. Try again.\n";
            }
        } while (op != 3); // Exit on option 3

    }
    void viewResults() {
        Votesmenu();


    }


};

void voterSignup() {
    string province, city;

    // Fetch provinces
    string query1 = "SELECT name FROM Province";
    if (mysql_query(conn, query1.c_str()) != 0) {
        cout << "Error fetching provinces: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    cout << "\nAvailable Provinces:\n";
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        cout << row[0] << endl;
    }
    mysql_free_result(res); // Free after showing provinces

    province = Validation::getNonEmptyString("Enter your Province: ");
    // Convert province to lowercase
    transform(province.begin(), province.end(), province.begin(), ::tolower);
    // Validate province
    string checkProvinceQuery = "SELECT * FROM Province WHERE name= LOWER('" + province + "') ";
    if (mysql_query(conn, checkProvinceQuery.c_str()) != 0) {
        cout << "Error validating province: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    if (!res) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    if (mysql_num_rows(res) == 0) {
        cout << "Invalid province. Please try again.\n";
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res); // Free after checking province

    // Determine province ID manually
    int count = 0;
    if (province == "punjab") count = 1;
    else if (province == "sindh") count = 2;
    else if (province == "balochistan") count = 3;
    else if (province == "kpk") count = 4;

    // Fetch cities for selected province
    string query = "SELECT name FROM Cities WHERE p_id='" + to_string(count) + "'";
    if (mysql_query(conn, query.c_str()) != 0) {
        cout << "Error fetching cities: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    if (!res) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    cout << "\nAvailable Cities in '" + province + "' are:\n";
    while ((row = mysql_fetch_row(res))) {
        cout << row[0] << endl;
    }
    mysql_free_result(res); // Free after showing cities

    city = Validation::getNonEmptyString("Enter your City: ");
    // Convert city to lowercase
    transform(city.begin(), city.end(), city.begin(), ::tolower);

    // Validate city
    string checkProvinceCityQuery = "SELECT * FROM Cities WHERE LOWER(name) = LOWER('" + city + "') AND p_id='" + to_string(count) + "'";
    if (mysql_query(conn, checkProvinceCityQuery.c_str()) != 0) {
        cout << "Error validating city: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    if (!res) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    if (mysql_num_rows(res) == 0) {
        cout << "Invalid city. Please try again.\n";
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res); // Free after checking city

    // Insert new voter (dummy username and password assumed here)
    string Insertquery = "INSERT INTO Voters (username, password, city, role) VALUES ('', '', '" + city + "', 'voter')";
    if (mysql_query(conn, Insertquery.c_str()) == 0) {
        cout << "Signup successful! You can now login.\n";
    }
    else {
        cout << "Signup failed. Maybe username already exists.\n";
    }

    string getLastVoterQuery = "SELECT username, password FROM Voters ORDER BY id DESC LIMIT 1";
    if (mysql_query(conn, getLastVoterQuery.c_str()) == 0) {
        res = mysql_store_result(conn);
        if (res && mysql_num_rows(res) > 0) {
            row = mysql_fetch_row(res);
            cout << "\nYour Credentials:\n";
            cout << "Your Username: " << row[0] << endl;
            cout << "Your Password: " << row[1] << endl;
        }
        mysql_free_result(res);
    }
    else {
        cout << "Error fetching new voter credentials: " << mysql_error(conn) << endl;
    }

}


// Main Function

int main() {
    connectDatabase();


    while (true) {
        int type;
        cout << "\n1. Admin Login\n2. Voter Menu\n3. Candidate Login\n4. Exit \n";
        type = Validation::getValidatedInt("Choice: ", 1, 4);

        if (type == 4) break;

        if (type == 1) {
            Admin admin;
            if (admin.login()) {
                admin.menu();
            }
            else {
                cout << "Invalid credentials.\n";
            }
        }
        else if (type == 2) {
            int op;
            cout << "\n1. Sign Up\n2. Sign In\n";
            op = Validation::getValidatedInt("Choice: ", 1, 2);
            if (op == 1) {
                voterSignup();
            }
            else if (op == 2) {
                Voter voter;
                if (voter.login()) {
                    voter.menu();
                }
                else {
                    cout << "Invalid credentials.\n";
                }
            }
        }
        if (type == 3) {
            Candidate cand;
            if (cand.login()) {
                cand.menu();
            }
            else {
                cout << "Invalid credentials.\n";
            }
        }
    }


    mysql_close(conn);
    return 0;
}
