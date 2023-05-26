package Puzzles.ClassicPuzzles.Medium;

import java.util.*;
import java.util.function.BiPredicate;

/*
 * Shadows of the Knight - Episode 1
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class ShadowsOfTheKnightEpisode1 {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int W = in.nextInt(); // width of the building.
        int H = in.nextInt(); // height of the building.
        int N = in.nextInt(); // maximum number of turns before game over.
        int X0 = in.nextInt();
        int Y0 = in.nextInt();


        List<Window> targets = new ArrayList<>(W * H);
        for (int i = 0; i < W; i++)
            for (int j = 0; j < H; j++)
                targets.add(new Window(i, j));

        // game loop
        while (true) {
            final int X = X0, Y = Y0;
            targets.remove(new Window(X, Y));
            String bombDir = in.next(); // the direction of the bombs from batman's current location (U, UR, R, DR, D, DL, L or UL)
            BombDir bd = BombDir.findDir(bombDir);
            targets.removeIf(window -> !window.inHotZone(X, Y, bd));

            int XcmN = 0, YcmN = 0;
            for (Window w : targets) {
                XcmN += w.x;
                YcmN += w.y;
            }
            int minMoment = Integer.MAX_VALUE;
            Window jumpTo = targets.get(targets.size() / 2);
            for (Window w : targets) {
                w.calcMoment(XcmN, YcmN, targets.size());
                if (w.moment < minMoment) {
                    jumpTo = w;
                    minMoment = w.moment;
                }
            }
            X0 = jumpTo.x;
            Y0 = jumpTo.y;

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");

            // the location of the next window Batman should jump to.
            System.out.println(X0 + " " + Y0);
        }
    }

    static enum BombDir {

        UP        ("U",  (dx, dy) -> dx == 0 && dy  < 0),
        UPRIGHT   ("UR", (dx, dy) -> dx  > 0 && dy  < 0),
        RIGHT     ("R",  (dx, dy) -> dx  > 0 && dy == 0),
        DOWNRIGHT ("DR", (dx, dy) -> dx  > 0 && dy  > 0),
        DOWN      ("D",  (dx, dy) -> dx == 0 && dy  > 0),
        DOWNLEFT  ("DL", (dx, dy) -> dx  < 0 && dy  > 0),
        LEFT      ("L",  (dx, dy) -> dx  < 0 && dy == 0),
        UPLEFT    ("UL", (dx, dy) -> dx  < 0 && dy  < 0),
        ERROR     ("ER", (dx, dy) -> false);

        private final String bombDir;
        private final BiPredicate<Integer, Integer> detected;

        BombDir(String bombDir, BiPredicate<Integer, Integer> detected) {
            this.bombDir = bombDir;
            this.detected = detected;
        }

        static BombDir findDir(String bombDir) {
            for (BombDir bd : BombDir.values())
                if (bd.bombDir.equals(bombDir))
                    return bd;
            return BombDir.ERROR;
        }

        boolean inHotZone(int dx, int dy) {
            return detected.test(dx, dy);
        }

    }

    static class Window {

        final int x;
        final int y;
        int moment;

        Window(int x, int y) {
            this.x = x;
            this.y = y;
        }

        boolean inHotZone(int x0, int y0, BombDir bd) {
            return bd.inHotZone(x - x0, y - y0);
        }

        // "Almost" |Moment(x,y)|
        void calcMoment(int xcmN, int ycmN, int N) {
            moment = Math.abs(x * N - xcmN) + Math.abs(y * N - ycmN);
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            Window window = (Window) o;
            return x == window.x && y == window.y;
        }

    }

}