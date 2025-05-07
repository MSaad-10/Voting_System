#include <iostream>
#include <string>
#include <mysql.h>
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

// ====================================================================================

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

// ====================================================================================

// USER class (The parent class)

class User {
protected:
    string username, password, role;
public:
    virtual ~User() {}
    virtual bool login() = 0;
    virtual void menu() = 0;
};

// ====================================

// VOTER class

class Voter : public User {
public:
    bool login() override {
        username = Validation::getNonEmptyString("Username: ");
        password = Validation::getNonEmptyString("Password: ");
        string query = "SELECT role FROM Users WHERE username='" + username + "' AND password='" + password + "' AND role='voter'";
        mysql_query(conn, query.c_str());
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0)
        {
            return true;
        }
        return false;
    }

    void castVote() {
        if (!VotingControl::isVotingOpen()) {
            cout << "Voting is currently closed.\n";
            return;
        }

        // Get the voter's city
        string query = "SELECT city FROM Users WHERE username='" + username + "'";
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

        cout << "\nCandidates in your city (" << userCity << "):\n";

        // List candidates from user's city
        query = "SELECT id, party FROM Candidates WHERE city='" + userCity + "'";
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
        mysql_free_result(res); // Free after showing candidates

        // Now voting
        int voteId;
        cout << "\nEnter candidate ID to vote for: ";
        cin >> voteId;

        // Check if already voted
        string checkQuery = "SELECT * FROM Votes WHERE username='" + username + "'";
        mysql_query(conn, checkQuery.c_str());
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) > 0) {
            cout << "You have already voted!\n";
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);

        // Insert vote
        string voteQuery = "INSERT INTO Votes (username, candidate_id) VALUES ('" + username + "', " + to_string(voteId) + ")";
        if (mysql_query(conn, voteQuery.c_str()) != 0) {
            cout << "Error casting vote: " << mysql_error(conn) << endl;
            return;
        }

        // Update candidate votes
        string updateQuery = "UPDATE Candidates SET votes = votes + 1 WHERE id=" + to_string(voteId);
        mysql_query(conn, updateQuery.c_str());

        cout << " Vote cast successfully! Thank you for voting.\n";
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

// ============================================================================================


// CANDIDATE class

class Candidate : public User {
public:
    bool login() override {
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;
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

// ===================================================================================================

// ADMIN class

class Admin : public User {
public:
    bool login() override {
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;
        string query = "SELECT role FROM Users WHERE username='" + username + "' AND password='" + password + "' AND role='admin'";
        mysql_query(conn, query.c_str());
        res = mysql_store_result(conn);
        if (mysql_num_rows(res) > 0) return true;
        return false;
    }

    void addCandidate() {
        string party, city, name, title;

        cout << "Enter party name: "; cin >> party;
        cout << "Enter city name: "; cin >> city;
        string query = "INSERT INTO Candidates (party, city, name, title) VALUES ('" + party + "', '" + city + "', '" + name + "', '" + title + "')";
        mysql_query(conn, query.c_str());
        cout << "Candidate added successfully!\n";
    }

    void removeCandidate() {
        int id;
        cout << "Enter candidate ID to remove: ";
        cin >> id;
        string query = "DELETE FROM Candidates WHERE id=" + to_string(id);
        mysql_query(conn, query.c_str());
        cout << "Candidate removed successfully!\n";
    }

    void viewResults() {
        cout << "\nVoting Results:\n";
        string query = "SELECT id, city, party, votes FROM Candidates";
        mysql_query(conn, query.c_str());
        res = mysql_store_result(conn);
        while ((row = mysql_fetch_row(res))) {
            cout << "Candidate ID: " << row[0] << ", City: " << row[1] << ", Party: " << row[2] << ", Votes: " << row[3] << endl;
        }
    }

    void menu() override {
        int op;
        do {
            cout << "\nAdmin Menu\n1. Start Voting\n2. Stop Voting\n3. Add Candidate\n4. Remove Candidate\n5. View Results\n6. Logout\n";
            op = Validation::getValidatedInt("Choice: ", 1, 6);
            switch (op) {
            case 1: VotingControl::setVotingStatus(true); break;
            case 2: VotingControl::setVotingStatus(false); break;
            case 3: addCandidate(); break;
            case 4: removeCandidate(); break;
            case 5: viewResults(); break;
            }
        } while (op != 6);
    }
};

void voterSignup() {
    string username, password, city;
    cout << "Choose username: "; username = Validation::getNonEmptyString("Username: ");
    cout << "Choose password: "; password = Validation::getNonEmptyString("Password: ");
    cout << "Enter your city: "; city = Validation::getNonEmptyString("City: ");
    string query = "INSERT INTO Users (username, password, role, city) VALUES ('" + username + "', '" + password + "', 'voter', '" + city + "')";
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Signup successful! You can now login.\n";
    }
    else {
        cout << "Signup failed. Maybe username already exists.\n";
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