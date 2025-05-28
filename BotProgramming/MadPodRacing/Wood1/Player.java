package BotProgramming.MadPodRacing.Wood1;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Scanner;

class Player {

    Point player;
    Point nextCheckpoint;
    Point finishLine;
    Point opponent;
    int nextCheckpointDist;
    int nextCheckpointAngle;
    List<Point> checkpoints;
    int lap;
    int currentSegment;
    boolean firstSegment;
    List<Double> distanceSegments;
    int maxSegment;
    boolean boostAvailable;

    // race winning fine-tuning!
    final int radius = 600; // checkpoint radius
    final double reduceThreshold = 2.0; // actual breaking distance from checkpoint: radius * reduceThreshold ( >= 1.0 )
    final double thrustUponRadius = 0.6; // thrust upon reaching the checkpoint radius [0 .. 1.0]
    final double attenuationOnSlip = 1.0; // thrust attenuation when target checkpoint at cut-off degrees [0 .. 1.0]
    final int cutOffDegree = 115;
    final double angleAttenuationSharpness = 4.0;
    final double boostThreshold = 0.8; // activation upon reaching this fraction of the boost segment [0 .. 1.0]

    public Player() {
        player = new Point();
        nextCheckpoint = new Point();
        opponent = new Point();
        checkpoints = new ArrayList<>();
        lap = 0;
        firstSegment = false;
        boostAvailable = true;
    }

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        new Player().gameLoop(in);
        in.close();
    }

    @SuppressWarnings("InfiniteLoopStatement")
    private void gameLoop(Scanner in)
    {
        // game loop
        while (true) {
            /*
              Auto-generated code below aims at helping you parse
              the standard input according to the problem statement.
             */
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


            if (checkpoints.isEmpty()) {
                finishLine = (Point)player.clone();
                checkpoints.add(finishLine);
            }
            determineTrack();
            System.err.println("Lap: " + lap + " Segment: " + currentSegment);
            System.err.println("BOOST available on segment " + maxSegment + ": " + boostAvailable );
            System.err.println("Bearing: " + nextCheckpointAngle);
            System.err.println("Distance (in radius): " + (double)nextCheckpointDist/radius);
            System.out.println(output());
        }
    }

    private void determineTrack() {
        int s = checkpoints.size();
        Point next = (Point)nextCheckpoint.clone();
        currentSegment = checkpoints.indexOf(next);
        switch (currentSegment) {
            case -1:
                if (next.distanceTo(finishLine) <= radius) {
                    finishLine.setX(next.getX());
                    finishLine.setY(next.getY());

                    distanceSegments = new ArrayList<>(s);
                    for (int i = 0; i < s; i++)
                        distanceSegments.add(checkpoints.get(i).distanceTo(checkpoints.get((i + 1) % s)));
                    double maxDistance = 0.0, d;
                    for (int i = 0; i < s; i++)
                        if ((d = distanceSegments.get(i)) > maxDistance) {
                            maxDistance = d;
                            maxSegment = i + 1;
                        }
                }
                else
                    checkpoints.add(next);
                break;
            case 1:
                if (!firstSegment) {
                    firstSegment = true;
                    ++lap;
                }
                break;
            case 0:
                currentSegment = s;
                // fallthrough
            default:
                firstSegment = false;
        }
    }

    private String output() {
        if (boostAvailable && currentSegment == maxSegment && lap == 2
                && nextCheckpointDist < distanceSegments.get(currentSegment - 1) * boostThreshold) {
            boostAvailable = false;

            return nextCheckpoint.getX() + " " + nextCheckpoint.getY() + " BOOST";
        }
        double distFactor = Math.min(((1 - thrustUponRadius) * nextCheckpointDist
                - (1 - thrustUponRadius * reduceThreshold ) * radius) / radius / (reduceThreshold - 1), 1.0);
        nextCheckpointAngle = Math.abs(nextCheckpointAngle);
        if (nextCheckpointAngle > cutOffDegree)
            nextCheckpointAngle = cutOffDegree;
        nextCheckpointAngle -= cutOffDegree;
        double angleFactor = 1 - attenuationOnSlip
                / (1 + angleAttenuationSharpness * nextCheckpointAngle * nextCheckpointAngle);
        System.err.println("Angle attenuation: " + angleFactor);
        System.err.println("Distance attenuation: " + distFactor);
        int thrust = Math.min((int)(100 * distFactor * angleFactor), 100);

        return nextCheckpoint.getX() + " " + nextCheckpoint.getY() + " " + thrust;
    }

    private static class Point {

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
            return super.clone();
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
