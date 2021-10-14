@Library('bf-jenkins-utils@master')_

node('master') {
 stage('Determine Target Branch') {
  // The 'CHANGE_TARGET' env var is set by Jenkins and contains the value of the
  // branch the PR will be merged into if everything works well. We use this
  // variable to decide which p4factory docker image to use
  if (env.CHANGE_TARGET == null) {
     env.CHANGE_TARGET = "master"
  }

  target_branch = "${env.CHANGE_TARGET}"
 }
}

pipeline {
  agent { label 'github-pr-builder' }
  stages {
    stage('Pre processing: cleanup') {
      steps {
        script {
            stopPreviousRuns(this)
        }
      }
    }

    stage('Style Check') {
        parallel {
            stage ('Run clang-format-check.py ') {
                agent {
                    docker {
                    label 'github-pr-builder'
                        image "artifacts-bxdsw.sc.intel.com:9444/bf/p4factory:${target_branch}"
                        registryUrl 'https://artifacts-bxdsw.sc.intel.com:9444'
                        registryCredentialsId 'nexus-docker-creds'
                        args '--privileged --cap-add=ALL  --user=root'
                    }
                }

              steps {
                    sh '''
                      apt-get install -y clang-3.6 clang-format-3.6
                      git submodule update --init --recursive
                      if [ ! -d sandals ]; then
                       git clone git@github.com:barefootnetworks/sandals.git
                      fi
                      python sandals/jenkins/clang-format-check.py -r
                    '''
                }
            }
        } // parallel
    } // stage

  }
  post {
    always {
      script {
        if (currentBuild.currentResult == 'FAILURE') { // Other values: SUCCESS, UNSTABLE
          bfEmailFromGHId(this)
        }
      }
    }
  }
}


