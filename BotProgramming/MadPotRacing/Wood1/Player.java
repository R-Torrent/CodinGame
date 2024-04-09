package BotProgramming.MadPotRacing.Wood1;

import java.util.Scanner;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
class Player {

    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);

        // game loop
        while (true) {
            int x = in.nextInt();
            int y = in.nextInt();
            int nextCheckpointX = in.nextInt(); // x position of the next check point
            int nextCheckpointY = in.nextInt(); // y position of the next check point
            int nextCheckpointDist = in.nextInt(); // distance to the next checkpoint
            int nextCheckpointAngle = in.nextInt(); // angle between your pod orientation and the direction of the next checkpoint
            int opponentX = in.nextInt();
            int opponentY = in.nextInt();

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");


            // You have to output the target position
            // followed by the power (0 <= thrust <= 100)
            // i.e.: "x y thrust"
            int distFact = Math.min(50 * (nextCheckpointDist - 600) / 600, 100);
            if (nextCheckpointAngle > 90)
                nextCheckpointAngle = 90;
            else if (nextCheckpointAngle < -90)
                nextCheckpointAngle = -90;
            double angleFact = Math.cos(Math.PI * nextCheckpointAngle / 180);
            int thrust = Math.min((int)(distFact * angleFact), 100);
            System.out.println(nextCheckpointX + " " + nextCheckpointY + " " + thrust);
        }
    }
}