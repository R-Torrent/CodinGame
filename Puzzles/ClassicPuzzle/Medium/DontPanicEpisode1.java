package Puzzles.ClassicPuzzle.Medium;

import java.util.*;

/*
 * Don't Panic - Episode 1
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class DontPanicEpisode1 {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int nbFloors = in.nextInt(); // number of floors
        int width = in.nextInt(); // width of the area
        int nbRounds = in.nextInt(); // maximum number of rounds
        int exitFloor = in.nextInt(); // floor on which the exit is found
        int exitPos = in.nextInt(); // position of the exit on its floor
        int nbTotalClones = in.nextInt(); // number of generated clones
        int nbAdditionalElevators = in.nextInt(); // ignore (always zero)

        int[] elevators = new int[nbFloors];
        int nbElevators = in.nextInt(); // number of elevators
        for (int i = 0; i < nbElevators; i++) {
            int elevatorFloor = in.nextInt(); // floor on which this elevator is found
            int elevatorPos = in.nextInt(); // position of the elevator on its floor
            elevators[elevatorFloor] = elevatorPos;
        }

        int lastFloorBlocked = -1;
        // game loop
        while (true) {
            int cloneFloor = in.nextInt(); // floor of the leading clone
            int clonePos = in.nextInt(); // position of the leading clone on its floor
            String direction = in.next(); // direction of the leading clone: LEFT or RIGHT

            if (cloneFloor == -1) {
                System.out.println("WAIT");
                continue;
            }
            
            int floorTarget = cloneFloor == exitFloor ? exitPos : elevators[cloneFloor];

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");
            if (cloneFloor > lastFloorBlocked &&
                (direction.equals("RIGHT") && clonePos > floorTarget) ||
                    direction.equals("LEFT") && clonePos < floorTarget) {
                System.out.println("BLOCK");
                lastFloorBlocked = cloneFloor;
            }
            else
                System.out.println("WAIT"); // action: WAIT or BLOCK
        }
    }

}