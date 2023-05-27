package Puzzles.ClassicPuzzle.Easy;

import java.util.*;

/*
 * Power of Thor - Episode 1
 * Puzzles > Classic Puzzle > Easy
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 * ---
 * Hint: You can use the debug stream to print initialTX and initialTY, if Thor seems not follow your orders.
 **/
public class PowerOfThorEpisode1 {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int lightX = in.nextInt(); // the X position of the light of power
        int lightY = in.nextInt(); // the Y position of the light of power
        int initialTx = in.nextInt(); // Thor's starting X position
        int initialTy = in.nextInt(); // Thor's starting Y position

        // game loop
        while (true) {
            int remainingTurns = in.nextInt(); // The remaining amount of turns Thor can move. Do not remove this line.
            Dir d = Dir.direction(lightX - initialTx, lightY - initialTy);

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");

            // A single line providing the move to be made: N NE E SE S SW W or NW
            System.out.println(d.name);

            initialTx += d.diffX;
            initialTy += d.diffY;
        }
    }

    private enum Dir {

        NORTH ("N", 0, -1),
        NORTHEAST ("NE", 1, -1),
        EAST ("E", 1, 0),
        SOUTHEAST ("SE", 1, 1),
        SOUTH ("S", 0, 1),
        SOUTHWEST ("SW", -1, 1),
        WEST ("W", -1, 0),
        NORTHWEST ("NW", -1, -1);

        private final String name;
        private final int diffX, diffY;

        Dir(String name, int diffX, int diffY) {
            this.name = name;
            this.diffX = diffX;
            this.diffY = diffY;
        }

        public static Dir direction(int dirX, int dirY) {
            if (dirX != 0) dirX /= Math.abs(dirX);
            if (dirY != 0) dirY /= Math.abs(dirY);
            for (Dir d : Dir.values())
                if (d.diffX == dirX && d.diffY == dirY)
                    return (d);
            return (null);
        }

    }

}