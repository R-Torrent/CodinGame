package BotProgramming.MadPotRacing.Wood1;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Scanner;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
class Player {

    Point player;
    Point nextCheckpoint;
    Point opponent;
    int nextCheckpointDist;
    int nextCheckpointAngle;
    List<Point> checkpoints;
    int lap;
    int currentSegment;
    boolean newLap;
    List<Double> distanceSegments;
    int maxSegment;
    boolean boostAvailable;

    // game parameters
    final int radius = 600;
    final double reduceThreshold = 2.0; // >= 1.0
    final int thrustUponRadius = 40; // >= 0
    final double boostThreshold = 0.8; // [0 .. 1.0]
    final double attenuationOnSlip = 0.6; // [0 .. 1.0]

    public Player() {
        player = new Point();
        nextCheckpoint = new Point();
        opponent = new Point();
        checkpoints = new ArrayList<>();
        lap = 0;
        newLap = false;
        boostAvailable = true;
    }

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        new Player().gameLoop(in);
        in.close();
    }

    private void gameLoop(Scanner in)
    {
        // game loop
        while (true) {
            player.setX(in.nextInt());
            player.setY(in.nextInt());
            nextCheckpoint.setX(in.nextInt()); // x position of the next check point
            nextCheckpoint.setY(in.nextInt()); // y position of the next check point
            nextCheckpointDist = in.nextInt(); // distance to the next checkpoint
            nextCheckpointAngle = in.nextInt(); // angle between your pod orientation and the direction of the next checkpoint
            opponent.setX(in.nextInt());
            opponent.setY(in.nextInt());

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");


            // You have to output the target position
            // followed by the power (0 <= thrust <= 100)
            // i.e.: "x y thrust"
            determineTrack();
            System.out.println(output());
        }
    }

    private void determineTrack() {
        Point next = (Point)nextCheckpoint.clone();
        currentSegment = checkpoints.indexOf(next);
        switch (currentSegment) {
            case -1:
                checkpoints.add(next);
                break;
            case 0:
                if (!newLap) {
                    newLap = true;
                    if (++lap == 2) {
                        int s = checkpoints.size();
                        distanceSegments = new ArrayList<>(s);
                        for (int i = 0; i < s; i++)
                            distanceSegments.add(checkpoints.get(i).distanceTo(checkpoints.get((i + 1) % s)));
                        double maxDistance = 0.0, d;
                        for (int i = 0; i < s; i++)
                            if ((d = distanceSegments.get(i)) > maxDistance) {
                                maxDistance = d;
                                maxSegment = i;
                            }
                    }
                }
                break;
            default:
                newLap = false;
        }
    }

    private String output() {
        if (boostAvailable && currentSegment == maxSegment && lap == 2
                && nextCheckpointDist < distanceSegments.get(maxSegment) * boostThreshold) {
            boostAvailable = false;

            return nextCheckpoint.getX() + " " + nextCheckpoint.getY() + " BOOST";
        }
        int distFact = Math.min((int)(((100 - thrustUponRadius) * nextCheckpointDist
                - radius * (100 - reduceThreshold * thrustUponRadius)) / radius / (reduceThreshold - 1)), 100);
        if (nextCheckpointAngle > 90)
            nextCheckpointAngle = 90;
        else if (nextCheckpointAngle < -90)
            nextCheckpointAngle = -90;
        double angleFact = 1 + attenuationOnSlip * (Math.cos(Math.PI * nextCheckpointAngle / 180) - 1) / 2;
        int thrust = Math.min((int)(distFact * angleFact), 100);

        return nextCheckpoint.getX() + " " + nextCheckpoint.getY() + " " + thrust;
    }

    private class Point {

        private int x;
        private int y;

        double distanceTo(Point p) {
            return Math.sqrt((this.x - p.x) * (this.x - p.x) + (this.y - p.y) * (this.y - p.y));
        }

        private Point () { }

        private Point (int x, int y) {
            this.x = x;
            this.y = y;
        }

        @Override
        public Object clone() {
        try {
            return (Point)super.clone();
        } catch (CloneNotSupportedException e) {
            return new Point(this.x, this.y);
        } }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Point point = (Point) o;
            return x == point.x && y == point.y;
        }

        @Override
        public int hashCode() {
            return Objects.hash(x, y);
        }

        int getX() { return x; }

        int getY() { return y; }

        void setX(int x) { this.x = x; }

        public void setY(int y) { this.y = y; }

    }

}