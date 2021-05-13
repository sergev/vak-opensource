use std::time;

use rand::distributions::{Distribution, Uniform};
use rand::distributions::uniform::SampleUniform;

mod sort;
use self::sort::bubble;
use self::sort::quick;
use self::sort::shaker;

fn create_array<T: SampleUniform>(size: usize, min: T, max: T) -> Vec<T> {
    let mut result: Vec<T> = Vec::with_capacity(size);

    let between = Uniform::new(min, max);
    let mut rng = rand::thread_rng();

    for _ in 0..size {
        result.push(between.sample(&mut rng));
    }

    result
}


fn test_for_sorting<T: PartialOrd>(data: &Vec<T>) -> bool {
    for n in 1..data.len() {
        if data[n] < data[n - 1] {
            return false
        }
    }

    return true
}


fn run_sort<T, U>(mut data: &mut Vec<T>, algorithm: &U)
    where T: PartialOrd + Clone,
          U: sort::SortAlgorithm<T> + sort::Named
{
    println!("Algorithm: {}", algorithm.get_name());

    let now = time::Instant::now();
    algorithm.sort(&mut data);
    let duration = now.elapsed();

    if test_for_sorting(&data) {
        let time_ms = duration.as_secs() * 1000 + duration.subsec_millis() as u64;
        println!("    Sorted");
        println!("    Run {} ms", time_ms);
    }
    else {
        println!("    Unsorted!!!");
    }
}


fn main() {
    let size = 10000;
    let data = create_array(size, 0, 1000);
    println!("Data size: {}", size);

    let bubble_sort = bubble::BubbleSort::new();
    run_sort(&mut data.clone(), &bubble_sort);

    let shaker_sort = shaker::ShakerSort::new();
    run_sort(&mut data.clone(), &shaker_sort);

    let quick_sort_end = quick::QuickSortEnd::new();
    run_sort(&mut data.clone(), &quick_sort_end);

    let quick_sort_middle = quick::QuickSortMiddle::new();
    run_sort(&mut data.clone(), &quick_sort_middle);
}
