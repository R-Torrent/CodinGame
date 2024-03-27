package Challenge.SpringChallenge2024;

import java.util.*;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

public class Question5_PixelRendering {

    /**
     * @param n The size of the image
     * @param targetImage The rows of the desired image, from top to bottom
     */
    public static List<String> solve(int n, List<String> targetImage) {
        // Write your code here
        int[] whites = new int[n];
        int[] blacks = new int[n];
        List<String> stack = new ArrayList<>();

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (targetImage.get(i).charAt(j) == '.')
                    whites[i]++;
                else
                    blacks[j]++;

        int nRows = n;
        int nCols = n;
        while (true) {
            int colsWithBlack = 0;
            for (int j = 0; j < n; j++)
                if (blacks[j] > 0)
                    colsWithBlack++;
            if (colsWithBlack == 0) {
                Collections.reverse(stack);
                return stack;
            }
            for (int k = 0; k < n; k++) {
                if (whites[k] == nCols) {
                    stack.add("R " + k);
                    nRows--;
                    whites[k] = 0;
                    for (int j = 0; j < n; j++)
                        if (targetImage.get(k).charAt(j) == '#')
                            --blacks[j];
                    break;
                }
                if (blacks[k] == nRows) {
                    stack.add("C " + k);
                    nCols--;
                    blacks[k] = 0;
                    for (int i = 0; i < n; i++)
                        if (targetImage.get(i).charAt(k) == '.')
                            --whites[i];
                    break;
                }
            }
        }
    }

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     */
    public static void trySolution(List<String> commands) {
        System.out.println("" + gson.toJson(commands));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(solve(
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<List<String>>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}
