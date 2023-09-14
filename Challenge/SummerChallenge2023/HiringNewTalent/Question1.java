package Challenge.SummerChallenge2023.HiringNewTalent;

import java.util.*;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

public class Question1 {

    /**
     * @param mutantScores The score corresponding to each mutant
     * @param threshold The score threshold above which mutants should be ignored
     * @return
     */
    public static String bestRemainingMutant(Map<String, Double> mutantScores, int threshold) {
        // Write your code here
        Double maximum = mutantScores.values().stream().mapToDouble(d -> d)
                .filter(score -> score <= threshold)
                .max().orElseThrow(NoSuchElementException::new);

        return  mutantScores.entrySet().stream()
                .filter(entry -> maximum.equals(entry.getValue()))
                .map(Map.Entry::getKey)
                .findFirst()
                .get();
    }

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     * @param output
     */
    public static void trySolution(String output) {
        System.out.println("" + gson.toJson(output));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(bestRemainingMutant(
                    gson.fromJson(in.nextLine(), new TypeToken<Map<String, Double>>(){}.getType()),
                    gson.fromJson(in.nextLine(), new TypeToken<Integer>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}