-- SQL request(s) below
SELECT a.name, COUNT(agentid) AS count
FROM mutant m RIGHT JOIN agent a ON recruiterid = agentid
GROUP BY agentid
ORDER BY count DESC
LIMIT 10;