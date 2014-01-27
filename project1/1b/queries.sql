-- Define which database should be used
USE CS143;

-- Give me the names of all the actors in the movie 'Die Another Day'.
-- Please also make sure actor names are in this format: 
-- <firstname> <lastname>   (seperated by single space).

SELECT CONCAT(first, ' ', last)
FROM Actor,
	(
		SELECT aid
		FROM MovieActor
		WHERE mid =
		(
			SELECT id
			FROM Movie
			WHERE title = "Die Another Day"
		)
	
	) ActorID
WHERE id = ActorID.aid;

-- output appears:
-- CONCAT( first, ' ', last)
-- | John Cleese				|
-- | Paul Darrow				|
-- | Judi Dench					|
-- | Thomas Ho					|
-- | ... and so on				|


-- Give me the count of all the actors who acted in multiple movies.

SELECT COUNT(*)
FROM
	(
		SELECT aid
		FROM MovieActor
		GROUP BY aid
		HAVING COUNT(*) >= 2
	) A;


-- Custom
-- Personal query
-- Display Year Released and Title for a given Genre, "Comedy"
-- Display in the ascending order of year released
SELECT year, title
FROM Actor A, Movie M, MovieActor MA, MovieGenre MG
WHERE M.id = MA.mid AND A.id = MA.aid AND MG.mid = M.id AND MG.genre = 'Comedy'
GROUP BY MA.mid
ORDER BY M.year ASC;