package Puzzles.ClassicPuzzle.Easy;

import java.util.*;
import java.util.stream.IntStream;

/*
 * Horse-Racing Duals
 * Puzzles > Classic Puzzle > Easy
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class HorseRacingDuals {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int N = in.nextInt();

        List<Integer> strengths = new ArrayList<>(N);
        for (int i = 0; i < N; i++) {
            int pi = in.nextInt();
            strengths.add(pi);
        }
        Collections.sort(strengths);
        OptionalInt closest = IntStream.range(0, N - 1).map(i -> strengths.get(i + 1) - strengths.get(i)).min();

        // Write an answer using System.out.println()
        // To debug: System.err.println("Debug messages...");

        closest.ifPresent(System.out::println);
        in.close();
    }

}