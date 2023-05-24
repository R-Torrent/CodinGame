package Puzzles.ClassicPuzzles.Easy;

import java.util.*;

/*
 * Temperatures
 * Puzzles > Classic Puzzle > Easy
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
class Temperatures {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int n = in.nextInt(); // the number of temperatures to analyse
        if (n == 0) {
            System.out.println("0");
            return;
        }
        int closest = Integer.MAX_VALUE;
        int absClosest = closest;
        for (int i = 0; i < n; i++) {
            int t = in.nextInt(); // a temperature expressed as an integer ranging from -273 to 5526
            int abst = Math.abs(t);
            if (abst < absClosest || (abst == absClosest && t > 0)) {
                closest = t;
                absClosest = Math.abs(closest);
            }
        }

        // Write an answer using System.out.println()
        // To debug: System.err.println("Debug messages...");

        System.out.println(closest);
    }

}