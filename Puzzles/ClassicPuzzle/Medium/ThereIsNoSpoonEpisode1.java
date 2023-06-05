package Puzzles.ClassicPuzzle.Medium;

import java.util.*;

/*
 * There Is No Spoon - Episode 1
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Don't let the machines win. You are humanity's last hope...
 **/
public class ThereIsNoSpoonEpisode1 {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int width = in.nextInt(); // the number of cells on the X axis
        int height = in.nextInt(); // the number of cells on the Y axis

        boolean[][] nodes = new boolean[width][height];

        if (in.hasNextLine()) {
            in.nextLine();
        }

        for (int i = 0; i < height; i++) {
            String line = in.nextLine(); // width characters, each either 0 or .
            for (int j = 0; j < width; j++)
                nodes[j][i] = line.charAt(j) == '0';
        }

        // Write an action using System.out.println()
        // To debug: System.err.println("Debug messages...");

        StringJoiner sj;
        // Three coordinates: a node, its right neighbor, its bottom neighbor
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                if (nodes[j][i]) {
                    sj = new StringJoiner(" ");
                    sj.add(String.valueOf(j));
                    sj.add(String.valueOf(i));
                    int j1;
                    for (j1 = j + 1; j1 < width; j1++)
                        if (nodes[j1][i])
                            break;
                    sj.add(String.valueOf ((j1 != width) ? j1 : -1));
                    sj.add(String.valueOf ((j1 != width) ? i : -1));
                    int i1;
                    for (i1 = i + 1; i1 < height; i1++)
                        if (nodes[j][i1])
                            break;
                    sj.add(String.valueOf ((i1 != height) ? j : -1));
                    sj.add(String.valueOf ((i1 != height) ? i1 : -1));
                    System.out.println(sj.toString());
                }

        in.close();
    }

}