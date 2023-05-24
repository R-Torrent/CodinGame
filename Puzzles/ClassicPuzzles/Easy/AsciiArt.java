package Puzzles.ClassicPuzzles.Easy;

import java.util.*;

/*
 * ASCII Art
 * Puzzles > Classic Puzzle > Easy
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class AsciiArt {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int L = in.nextInt();
        int H = in.nextInt();
        if (in.hasNextLine()) {
            in.nextLine();
        }
        String T = in.nextLine();
        String[] rows = new String[H];
        for (int i = 0; i < H; i++) {
            rows[i] = in.nextLine();
        }

        // Write an answer using System.out.println()
        // To debug: System.err.println("Debug messages...");

        StringBuilder row;
        for (int i = 0; i < H; i++) {
            row = new StringBuilder();
            for (char c : T.toCharArray()) {
                c = Character.toUpperCase(c);
                int index;
                if ('A' <= c && c <= 'Z')
                    index = (c - 'A') * L;
                else // '?'
                    index = 26 * L;
                row.append(rows[i].substring(index, index + L));
            }
            System.out.println(row.toString());
        }
    }

}