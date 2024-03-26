package Challenge.SpringChallenge2024;

import java.util.*;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

public class Question4_PatternDetection {

    /**
     * @param nRows The height of the image.
     * @param nCols The width of the image.
     * @param image Pixels of the image, given row by row from top to bottom.
     * @return The parameters of the largest circle [centerRow, centerCol, radius].
     */
    public static List<Integer> findLargestCircle(int nRows, int nCols, List<String> image) {
        // Write your code here
        int x_sol = 0, y_sol = 0, r_sol = 0;

        for (int i = 0; i < nRows; i++)
            for (int j = 0; j < nCols; j++) {
                int rmax = Math.min(Math.min(i, nRows - i - 1), Math.min(j, nCols - j - 1));
                for (int r = rmax; r >= 0; r--) {
                    if (r <= r_sol)
                        break;
                    if (checkBox(image, r, i, j)) {
                        x_sol = i;
                        y_sol = j;
                        r_sol = r;
                    }
                }
            }
        return Arrays.asList(x_sol, y_sol, r_sol);
    }

    private static boolean checkBox(List<String> image, final int r, final int i, final int j) {
        final int i1 = i - r, i2 = i + r, j1 = j - r, j2 = j + r;
        final int r2 = r * r;
        final int s2 = r2 + (r << 1) + 1;
        int color;

        if ((color = image.get(i).charAt(j1)) == image.get(i).charAt(j2) &&
                color == image.get(i1).charAt(j) && color == image.get(i2).charAt(j)) {
            for (int m = i1; m <= i2; m++) {
                if (m == i)
                    continue;
                for (int n = j1; n <= j2; n++) {
                    if (n == j)
                        continue;
                    int x2 = (m - i) * (m - i);
                    int y2 = (n - j) * (n - j);
                    int z2 = x2 + y2;
                    if (r2 <= z2 && z2 < s2 && image.get(m).charAt(n) != color)
                        return false;
                }
            }
            return true;
        }
        return false;
    }

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     * @param largestCircle The parameters of the largest circle [centerRow, centerCol, radius].
     */
    public static void trySolution(List<Integer> largestCircle) {
        System.out.println("" + gson.toJson(largestCircle));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(findLargestCircle(
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<List<String>>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}
