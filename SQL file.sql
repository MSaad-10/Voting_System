-- Create the database
CREATE DATABASE voting_system;
USE voting_system;

-- Table for users (admin and voters)
CREATE TABLE Users (
    username VARCHAR(50) PRIMARY KEY,
    password VARCHAR(50) NOT NULL,
    role ENUM('admin') NOT NULL,
    city VARCHAR(50)  -- Only for voters, admin can have empty city
);
select * from Users;

CREATE TABLE Voters (
	username VARCHAR(50) PRIMARY KEY,
    password VARCHAR(50) NOT NULL,
    role ENUM('voter') NOT NULL,
    city VARCHAR(50) NOT NULL,
    MNA_vote BOOLEAN DEFAULT (0),
    MPA_vote BOOLEAN DEFAULT (0),
    vote_status BOOLEAN DEFAULT (0)
    );

select * from Voters;


    
	


-- Table for candidates
CREATE TABLE Candidates (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(50) NOT NULL unique,
    party VARCHAR(50) NOT NULL,
    city VARCHAR(50) NOT NULL,
    votes INT DEFAULT 0,
    title ENUM ('MPA', 'MNA') NOT NULL
);
select * from Candidates;




-- Table for votes
CREATE TABLE Votes (
    username VARCHAR(50) PRIMARY KEY,
    candidate_id INT,
    FOREIGN KEY (username) REFERENCES Voters(username),
    FOREIGN KEY (candidate_id) REFERENCES Candidates(id)
);
select * from votes;



-- Table for controlling voting status
CREATE TABLE VotingStatus (
    id INT PRIMARY KEY,
    is_open BOOLEAN
);
select * from VotingStatus;



-- Insert initial voting status (closed by default)
INSERT INTO VotingStatus (id, is_open) VALUES (1, 1);


-- INSERTING DATA INTO USERS
INSERT INTO Users (username, password, role, city) VALUES
('admin', 'admin123', 'admin', '');

-- INSERTING DATA INTO VOTERS 
INSERT INTO Voters (username, password, role, city, MNA_vote, MPA_vote, Vote_status) VALUES
('voter1', 'pass1', 'voter', 'Lahore', 0, 0, 0),
('voter2', 'pass2', 'voter', 'Karachi', 0, 0, 0),
('voter3', 'pass3', 'voter', 'Peshawar', 0, 0, 0),
('voter4', 'pass4', 'voter', 'Quetta', 0, 0, 0),
('voter5', 'pass5', 'voter', 'Gilgit', 0, 0, 0),
('voter6','pass6','voter','Islamabad', 0, 0, 0);


-- INSERTING DATA INTO CANDIDATES
INSERT INTO Candidates (username, password, party, city, title) VALUES
('candidate1', 'pass123', 'Party A', 'Lahore', 'MNA'),
('candidate2', 'pass456', 'Party B', 'Lahore', 'MPA'),
('candidate3', 'pass789', 'Party A', 'Karachi', 'MNA'),
('candidate4', 'pass101', 'Party B', 'Karachi', 'MPA'),
('candidate5', 'pass112', 'Party A', 'Karachi', 'MNA'),
('candidate6', 'pass131', 'Party B', 'Lahore', 'MPA'),
('candidate7', 'pass415', 'Party A', 'Quetta', 'MNA'),
('candidate8', 'pass161', 'Party C', 'Quetta', 'MPA'),
('candidate9', 'pass718', 'Party B', 'Gilgit', 'MNA'),
('candidate10', 'pass191', 'Party C', 'Gilgit', 'MPA');

drop database voting_system;