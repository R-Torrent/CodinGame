package Challenge.SpringChallenge2024;

import java.awt.*;
import java.util.*;
import java.util.List;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

public class Question6_PatternDetection {

    /**
     * @param nRows The height of the image.
     * @param nCols The width of the image.
     * @param image Pixels of the image, given row by row from top to bottom. All pixel colors are alphanumeric.
     * @return The total length of wire needed to deploy the network (modulo 10^9+7)
     */
    public static int getCableLength(int nRows, int nCols, List<String> image) {
        // Write your code here
        long res = 0;
        final int mod = 1_000_000_007;
        record Point(int x, int y) {
            private static int compareByCol(Point p1, Point p2) { return p1.y - p2.y; }
        }
        Map<Character, List<Point>> found = new TreeMap<>();

        for (int i = 0; i < nRows; i++)
            for (int j = 0; j < nCols; j++) {
                char c = image.get(i).charAt(j);
                if (!found.containsKey(c))
                    found.put(c, new ArrayList<>());
                found.get(c).add(new Point(i, j));
            }
        for (List<Point> l : found.values()) {
            long wires = 0, x_t = 0, nx = 0;
            for (Point p : l) {
                wires += nx * p.x - x_t;
                x_t += p.x;
                nx++;
            }
            l.sort(Point::compareByCol);
            long y_t = 0, ny = 0;
            for (Point p : l) {
                wires += ny * p.y - y_t;
                y_t += p.y;
                ny++;
            }
            res += wires << 1;
        }

        return (int)(res % mod);
    }

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     * @param cableLength The total length of wire needed to deploy the network (modulo 10^9+7)
     */
    public static void trySolution(int cableLength) {
        System.out.println("" + gson.toJson(cableLength));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(getCableLength(
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<List<String>>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}
