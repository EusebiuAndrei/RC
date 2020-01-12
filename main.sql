-- insert

INSERT INTO users VALUES(1, 'admin', 'Matt', 'abc');
INSERT INTO songs (title, description, link, genres)
 VALUES('Sea', 'A song about the sea', '@sea', 'pop');
INSERT INTO comments VALUES(1, 1, 1, 'Nice voice');

-- select
SELECT * FROM users;
SELECT * FROM songs;
SELECT * FROM comments;

-- delete
DELETE FROM users;
DELETE FROM songs;
DELETE FROM comments;

-- Create an user
-- createUser(role, username, password)
INSERT INTO users 
    VALUES(1, 'admin', 'Matt', 'abc');

-- Vote a song
-- voteSong(id_song)
UPDATE songs 
    SET votes = votes + 1
    WHERE id_song = 1;

-- Restrict the ability to vote
-- restrictVotes(id_user)
UPDATE users
    SET canVote = 0
    WHERE id_user = 1
    
-- Add a song
-- addSong(title, description, link, votes = 0, genres)
INSERT INTO songs 
    VALUES(1, 'Sea', 'A song about the sea', '@sea', 0, 'pop');

-- Remove a song
-- removeSong(id_song)
DELETE FROM songs
    WHERE id_song = 1;

-- Add a comment
-- addComment(id_user, id_song, text)
INSERT INTO comment
    VALUES(1, 1, 1, 'Nice voice');