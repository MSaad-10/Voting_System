-- Create the database
CREATE DATABASE voting_system;
USE voting_system;

-- Table for users (admin and voters)
CREATE TABLE Users (
    username VARCHAR(50) PRIMARY KEY,
    password VARCHAR(50) NOT NULL,
    role ENUM('admin', 'voter') NOT NULL,
    city VARCHAR(50)  -- Only for voters, admin can have empty city
);


select * from Users;

-- Table for candidates
CREATE TABLE Candidates (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
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
    FOREIGN KEY (username) REFERENCES Users(username),
    FOREIGN KEY (candidate_id) REFERENCES Candidates(id)
);

-- Table for controlling voting status
CREATE TABLE VotingStatus (
    id INT PRIMARY KEY,
    is_open BOOLEAN
);

select * from VotingStatus;

-- Insert initial voting status (closed by default)
INSERT INTO VotingStatus (id, is_open) VALUES (1, 0);
INSERT INTO Users (username, password, role, city) VALUES
('admin', 'admin123', 'admin', '');
INSERT INTO Users (username, password, role, city) VALUES
('voter1', 'pass1', 'voter', 'Lahore'),
('voter2', 'pass2', 'voter', 'Karachi'),
('voter3', 'pass3', 'voter', 'Peshawar'),
('voter4', 'pass4', 'voter', 'Quetta'),
('voter5', 'pass5', 'voter', 'Gilgit'),
('voter6','pass6','voter','Islamabad');
INSERT INTO Candidates (party, city) VALUES
('Party A', 'Lahore'),
('Party B', 'Lahore'),
('Party A', 'Karachi'),
('Party B', 'Karachi'),
('Party A', 'Peshawar'),
('Party B', 'Peshawar'),
('Party A', 'Quetta'),
('Party C', 'Quetta'),
('Party B', 'Gilgit'),
('Party C', 'Gilgit');

drop database voting_system;