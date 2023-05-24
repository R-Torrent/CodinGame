package Puzzles.ClassicPuzzles.Easy;

import java.util.*;

/*
 * Unary
 * Puzzles > Classic Puzzle > Easy
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class Unary {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        String message = in.nextLine();

        StringBuilder binary = new StringBuilder();
        char[] ascii = new char[7];
        for (char c : message.toCharArray()) {
            char one = 0b1000000;
            for (int i = 0; i < 7; i++, one >>>= 1)
                ascii[i] = (c & one) == one  ? '1' : '0';
            binary.append(ascii);
        }
        char[] bits = binary.toString().toCharArray();

        StringJoiner unary = new StringJoiner(" ");
        char current = '2';
        int size = 0;
        for (char bit : bits)
            if (bit == current)
                size++;
            else {
                convert(current, size, unary);
                current = bit;
                size = 1;
            }
        convert(current, size, unary);

        // Write an answer using System.out.println()
        // To debug: System.err.println("Debug messages...");

        System.out.println(unary.toString());
        in.close();
    }

    private static void convert(char bit, int size, StringJoiner unary)
    {
        if (size > 0) {
            unary.add(bit == '0' ? "00" : "0");
            unary.add("0".repeat(size));
        }
    }

}