-- SQL request(s) below
SELECT newColor color, COUNT(*) count
FROM pixels p
JOIN pixelUpdates u
ON p.id = u.pixelId
WHERE firstPaintedAt <> updatedAt
GROUP BY color
ORDER BY count DESC