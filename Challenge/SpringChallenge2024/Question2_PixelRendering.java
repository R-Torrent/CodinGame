package Challenge.SpringChallenge2024;

import java.util.*;

public class Question2_PixelRendering {

    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);
        int n = in.nextInt();
        if (in.hasNextLine()) {
            in.nextLine();
        }

        String[] canvas = new String[n];
        for (int i = 0; i < n; i++) {
            canvas[i] = ".".repeat(n);
        }
        // game loop
        while (true) {
            String command = in.nextLine();
            if (command.isEmpty())
                continue;
            char type = command.charAt(0);
            int coord = Integer.parseInt(command.substring(2));
            StringBuilder newRow;

            if (type == 'R')
                canvas[coord] = ".".repeat(n);
            for (int i = 0; i < n; i++) {

                // Write an action using System.out.println()
                // To debug: System.err.println("Debug messages...");

                if (type == 'C') {
                    newRow = new StringBuilder(canvas[i]);
                    newRow.setCharAt(coord, '#');
                    canvas[i] = newRow.toString();
                }

                // Print the i-th line of the image after the command was executed
                System.out.println(canvas[i]);
            }
        }
    }

}
