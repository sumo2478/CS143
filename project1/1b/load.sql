-- USE CS143; -- TODO: Change to CS143
-- TODO: Might have to add in tuples for MaxMovieID and MaxPersonID

-- Load Movies into Database
LOAD DATA LOCAL INFILE './data/movie.del' 
INTO TABLE Movie 
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

-- Load Actors into Database
LOAD DATA LOCAL INFILE './data/actor1.del'
INTO TABLE Actor
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

LOAD DATA LOCAL INFILE './data/actor2.del'
INTO TABLE Actor
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

LOAD DATA LOCAL INFILE './data/actor3.del'
INTO TABLE Actor
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

-- Load Directors into Database
LOAD DATA LOCAL INFILE './data/director.del'
INTO TABLE Director
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

-- Load Movie Genre
LOAD DATA LOCAL INFILE './data/moviegenre.del'
INTO TABLE MovieGenre
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

-- Load Movie Directors
LOAD DATA LOCAL INFILE './data/moviedirector.del'
INTO TABLE MovieDirector
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

-- Load Movie Actor
LOAD DATA LOCAL INFILE './data/movieactor1.del'
INTO TABLE MovieActor
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';

LOAD DATA LOCAL INFILE './data/movieactor2.del'
INTO TABLE MovieActor
FIELDS OPTIONALLY ENCLOSED BY '"'
TERMINATED BY ',';