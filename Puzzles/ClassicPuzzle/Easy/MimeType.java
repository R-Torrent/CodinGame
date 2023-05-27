package Puzzles.ClassicPuzzle.Easy;

import java.util.*;

/*
 * MIME Type
 * Puzzles > Classic Puzzle > Easy
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class MimeType {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int N = in.nextInt(); // Number of elements which make up the association table.
        int Q = in.nextInt(); // Number Q of file names to be analyzed.

        List<Extension> extensions = new ArrayList<>(N);
        List<String> files = new ArrayList<>(Q);

        for (int i = 0; i < N; i++) {
            String EXT = in.next(); // file extension
            String MT = in.next(); // MIME type.
            extensions.add(new Extension(EXT, MT));
        }
        Collections. sort(extensions);
        in.nextLine();
        for (int i = 0; i < Q; i++) {
            String FNAME = in.nextLine(); // One file name per line.
            files.add(FNAME);
        }

        // Write an answer using System.out.println()
        // To debug: System.err.println("Debug messages...");

        // For each of the Q filenames, display on a line the corresponding MIME type. If there is no corresponding type, then display UNKNOWN.
        for (String file : files)
            Optional.of(file)
                    .filter(f -> f.contains("."))
                    .map(f -> f.substring(file.lastIndexOf('.') + 1))
                    .map(str -> Collections.binarySearch(extensions, new Extension(str, null)))
                    .filter(idx -> idx >= 0)
                    .map(extensions::get)
                    .ifPresentOrElse(extension -> System.out.println(extension.mt), () -> System.out.println("UNKNOWN"));
        in.close();
    }

    static class Extension implements Comparable<Extension> {

        final String ext;
        final String mt;

        Extension(String ext, String mt) {
            this.ext = ext;
            this.mt = mt;
        }

        @Override
        public int compareTo(Extension o) {
            return ext.toUpperCase().compareTo(o.ext.toUpperCase());
        }

    }

}